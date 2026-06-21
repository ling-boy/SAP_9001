/**
 * @file modbus_9001.cpp
 * @brief Modbus 9001 IoT网关主程序
 * @author 朱柴林
 * @date 2026-06-21
 * @version 3.0.0
 *
 * @details SAP_9001 V3.0 物联网网关系统
 *
 * 核心功能：
 *   - 多种通信方式管理（LoRa、蓝牙、WiFi、4G、LAN）
 *   - 传感器数据采集与处
 *   - HJ212协议数据封装与上报
 *   - 设备注册与心跳维护
 *   - GPS定位数据获取
 *
 * 架构特点：
 *   - 8种设计模式：单例、工厂、策略、建造者、状态机、模板方法、观察者、责任链
 *   - 现代C++14特性：智能指针、lambda、std::function、内存池
 *   - 分布式容错：指数退避、熔断器、心跳机制
 *   - 线程安全：DeviceContext单例管理全局状态
 *
 * 编译环境：
 *   - GCC 13 ARM 交叉编译工具链
 *   - C++14 标准
 *   - CMake 3.16+
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string>
#include <vector>
#include <queue>
#include <errno.h>
#include <signal.h>
#include "infra/get.h"
#include "protocol/protocol_process.h"
#include "infra/software_wdt.h"
#include "infra/communica_manage.h"
#include "app/regist_manage.h"
#include "hal/gps.h"
#include "app/device_init.h"
#include "app/message_transmitter.h"
#include "sensor/sensor_driver.h"
#include "infra/message_queue.h"
#include "infra/logger.h"
#include "infra/config.h"
#include "core/device_context.h"

// 使用内联函数替代宏，避免 .c_str() 悬空指针风险
static std::string getWatchPath() {
    return CFG_STR("watchdog", "pid_file", "/home/root/myWatch.txt");
}
// 从配置文件读取看门狗超时时间
static int getSoftdogTimeout() {
    return CFG_INT("watchdog", "timeout_sec", 30);
}

/* 所有全局状态已迁移至 DeviceContext 单例，通过 sap::DeviceContext::instance() 访问 */

/* 信号处理器使用的静态缓存指针（async-signal-safe 要求不能调用 DeviceContext::instance()） */
static MessageQueue<std::string>* g_pTransMessage = nullptr;

/** @brief 写PID文件的fd，供信号处理函数使用 */
static int g_fdWatch = -1;

/**
 * @brief 信号处理公共清理函数（async-signal-safe）
 * @details 持久化未发送数据，写入PID=0通知看门狗脚本，然后退出进程。
 *          所有操作均为 async-signal-safe 函数。
 */
static void signal_cleanup_and_exit()
{
    /* 持久化未发送的数据（无锁版本，避免信号处理器中死锁） */
    if (g_pTransMessage)
        drainAndPersistUnsafe(*g_pTransMessage);
    /* 写入PID=0通知看门狗脚本（lseek/write/close/ftruncate 为 async-signal-safe） */
    if (g_fdWatch >= 0) {
        lseek(g_fdWatch, 0, SEEK_SET);
        const char* zero = "0";
        write(g_fdWatch, zero, 1);
        ftruncate(g_fdWatch, 1);
        close(g_fdWatch);
    }
    /* _exit() 由 OS 回收所有资源，无需手动释放 */
    _exit(0);
}

/**
 * @brief 信号处理函数，持久化未发送数据后退出
 * @details 处理 SIGTERM/SIGINT
 */
static void signal_handler(int signum)
{
    (void)signum;
    signal_cleanup_and_exit();
}

/**
 * @brief 看门狗超时信号处理函数
 * @details 处理 SIGUSR1，当看门狗检测到线程超时时触发
 */
static void watchdog_signal_handler(int signum)
{
    (void)signum;
    signal_cleanup_and_exit();
}

/**
 * @brief 将PID写入看门狗文件
 */
static void write_pid_file(const std::string& path, const std::string& value)
{
    int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        write(fd, value.c_str(), value.length());
        close(fd);
    }
}

int main()
{
    /* 初始化日志系统 */
    sap::Logger::instance().setLevel(sap::LogLevel::DEBUG);
    LOG_INFO("main", "SAP_9001 V2.1 starting...");

    /* 加载配置文件 */
    if (!sap::Config::instance().load("./config/device.conf")) {
        LOG_WARN("main", "Config file not found, using defaults");
    } else {
        LOG_INFO("main", "Config loaded from ./config/device.conf");
    }

    /* 初始化 DeviceContext 单例并缓存信号处理器所需的指针 */
    auto& ctx = sap::DeviceContext::instance();
    g_pTransMessage = &ctx.queues().transmit;

    /* 保存主线程 ID 到 DeviceContext，供看门狗超时时发送信号 */
    ctx.threads().main = pthread_self();

    /* 注册信号处理函数，确保被 kill 时能持久化数据 */
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    /* 注册看门狗超时信号处理函数 */
    struct sigaction sa_wdt;
    sa_wdt.sa_handler = watchdog_signal_handler;
    sigemptyset(&sa_wdt.sa_mask);
    sa_wdt.sa_flags = 0;
    sigaction(SIGUSR1, &sa_wdt, NULL);

    pid_t pt = getpid();
    std::string ptstr = std::to_string(pt);
    g_fdWatch = open(getWatchPath().c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (g_fdWatch < 0) LOG_ERROR("main", "open watch file failed: %s", strerror(errno));
    if (g_fdWatch >= 0) {
        int ret = write(g_fdWatch, ptstr.c_str(), ptstr.length());
        if (ret < 0) {
            LOG_ERROR("main", "write PID failed: %s", strerror(errno));
        }
        /* 保持 g_fdWatch 打开，信号处理器会 lseek 到开头后写入 "0" */
    }

    std::string power_script = CFG_STR("paths", "power_script", "./power_12v.sh");
    std::string power_on_cmd = power_script + " on";
    system(power_on_cmd.c_str());
    sleep(1);

    if (pthread_create(&ctx.threads().gps, NULL, GET_GPS, NULL) != 0) {
        LOG_ERROR("main", "pthread_create tid_gps failed");
    }
    sleep(1);

    int ret = dev_init();
    if (-1 == ret) {
        LOG_FATAL("main", "Init communicate device error, start reboot!");
        /* 硬件清理：关闭LED、关闭12V电源、同步RTC */
        system("echo 0 > /sys/class/leds/red/brightness");
        system("echo 0 > /sys/class/leds/yellow/brightness");
        system("echo 0 > /sys/class/leds/green/brightness");
        std::string power_script = CFG_STR("paths", "power_script", "./power_12v.sh");
        std::string power_off_cmd = power_script + " off";
        system(power_off_cmd.c_str());
        system("hwclock -w");
        system("killall wpa_supplicant");
        sleep(1);
        system("killall udhcpc");
        write_pid_file(getWatchPath().c_str(), "0");
        LOG_INFO("main", "Process exiting");
        return 0;
    }

    LOG_INFO("main", "Init communicate device success");
    sleep(1);
    get_mac();
    ctx.setIdentityCpuOccupy(get_cpuOccupy());
    LOG_INFO("main", "CPU Usage Rate: %s", ctx.getIdentityCpuOccupy().c_str());
    std::vector<std::vector<int>> vec;
    vec = ctx.commManager().getALLIfd();
    deviceRegist* registObj = new deviceRegist(&ctx.softwareWdt(), &vec);
    if (pthread_create(&ctx.threads().device_regist, NULL, device_regist, registObj) != 0) {
        LOG_ERROR("main", "pthread_create tid_deviceRegist failed");
        ctx.threads().device_regist = 0;
    }
    if (pthread_create(&ctx.threads().watchdog, NULL, softwarewd, &ctx.softwareWdt()) != 0) {
        LOG_ERROR("main", "pthread_create tid_softwd failed");
        ctx.threads().watchdog = 0;
    }
    void* regRet;
    if (ctx.threads().device_regist != 0 && pthread_join(ctx.threads().device_regist, &regRet) == 0)
    {
        delete registObj;
        if (regRet == (void*)-1) {
            LOG_ERROR("main", "Device registration failed");
            sleep(30);
            write_pid_file(getWatchPath().c_str(), "0");
            return 0;
        }
    } else {
        delete registObj;
    }
    LOG_INFO("main", "Device Registration Successful");
    int numRead = readFromFile(ctx.queues().offline_cache);
    LOG_INFO("main", "Read unsent packets: %d", numRead);
    sleep(1);
    if (pthread_create(&ctx.threads().ship_data, NULL, get_ship_data, &ctx.softwareWdt()) != 0) {
        LOG_ERROR("main", "pthread_create tid_ship_data failed");
        ctx.threads().ship_data = 0;
    }
    if (pthread_create(&ctx.threads().trans_message, NULL, send_mess, &ctx.softwareWdt()) != 0) {
        LOG_ERROR("main", "pthread_create tid_transMessage failed");
        ctx.threads().trans_message = 0;
    }

    /* 看门狗线程设为分离态，不再 join（它永远不会退出） */
    if (ctx.threads().watchdog != 0) {
        pthread_detach(ctx.threads().watchdog);
    }

    /* 主线程等待信号，信号处理函数负责持久化数据和清理 */
    LOG_INFO("main", "Main thread waiting for signal...");
    pause();

    /* 不会到达此处，信号处理函数中调用 _exit(0) */
    return 0;
}
