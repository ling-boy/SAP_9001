/**
 * @file modbus_9001.cpp
 * @brief Modbus 9001 IoT网关主程序
 * @author 朱柴林
 * @date 2026-06-19
 * @version 2026
 *
 * @details 实现物联网网关的核心功能，包括：
 *          - 多种通信方式管理（LoRa、蓝牙、WiFi、4G、LAN）
 *          - 传感器数据采集与处理
 *          - HJ212协议数据封装与上报
 *          - 设备注册与心跳维护
 *          - GPS定位数据获取
 */
#include <stdio.h>
#include <iostream>
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

#define watchPathName "/home/root/myWatch.txt"
using namespace std;
extern const int softdogTimeout = 30;

/* 通信设备文件描述符 */
int fd_lora = -1, fd_wifi = -1, fd_bt = -1, fd_lan = -1, fdL_lan = -1;
vector<int>device_id;
string id = "FF", net_id = "0000", mac = "", bt_mac = "", Isr_mac = "";
int monitor_time = 4;
string current_time = "";
char communicate_status[] = "0000";
unsigned int hj_crc;

/* 线程同步控制变量 */
int iS_getsensor = 0; int iS_getshipsensor = 0;
pthread_mutex_t mtx_sensor = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_sensor = PTHREAD_COND_INITIALIZER;

/* 通信设备管理类 */
communicaManage* CM = new communicaManage;

pthread_t tid_getSensor, tid_getLan, tid_transMessage, tid_gps, tid_ship_data, tid_softwd, tid_monitor485, tid_deviceRegist;
int wdtNewSensorId = -1, wdtShipId = -1;
MessageQueue<string> transMessage;
queue<string>readMessage;
string cpu_occupy = "";

int flag_wifi = 0;

struct sockaddr_in wifi_ser_addr;
struct sockaddr_in lan_ser_addr;
struct sockaddr_in Llan_ser_addr;
struct sockaddr_in Llan_cli_addr;

/** @brief 写PID文件的fd，供信号处理函数使用 */
static int g_fdWatch = -1;

/** @brief 全局停止标志位，由 stop() 设置，当前仅用于 dev_init 失败路径 */
volatile sig_atomic_t g_stop_flag = 0;

/**
 * @brief 信号处理函数，持久化未发送数据后退出
 * @details 处理 SIGTERM/SIGINT，将 transMessage 队列中的数据写入文件，
 *          关闭LED、同步RTC，然后退出进程
 */
static void signal_handler(int signum)
{
    (void)signum;
    /* 持久化未发送的数据（无锁版本，避免信号处理器中死锁） */
    drainAndPersistUnsafe(transMessage);
    /* 写入PID=0通知看门狗脚本（lseek/write/close 为 async-signal-safe） */
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
 * @brief 将PID写入看门狗文件
 */
static void write_pid_file(const string& path, const string& value)
{
    int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        write(fd, value.c_str(), value.length());
        close(fd);
    }
}

int main()
{

    printf("朱柴林朱柴林朱柴林朱柴林朱柴林\n");
    printf("朱柴林朱柴林朱柴林朱柴林朱柴林\n");
    printf("朱柴林朱柴林朱柴林朱柴林朱柴林\n");
    printf("朱柴林朱柴林朱柴林朱柴林朱柴林\n");
    printf("朱柴林朱柴林朱柴林朱柴林朱柴林\n");
    /* 注册信号处理函数，确保被 kill 时能持久化数据 */
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    pid_t pt = getpid();
    string ptstr = to_string(pt);
    g_fdWatch = open(watchPathName, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (g_fdWatch < 0) perror("main open:");
    if (g_fdWatch >= 0) {
        int ret = write(g_fdWatch, ptstr.c_str(), ptstr.length());
        if (ret < 0) {
            perror("错误：");
        }
        /* 保持 g_fdWatch 打开，信号处理器会 lseek 到开头后写入 "0" */
    }

    system("./power_12v.sh on");
    sleep(1);
    CSoftwareWdt softwareWdt;
    CSoftwareWdt* g_CsoftwareWdt = &softwareWdt;

    if (pthread_create(&tid_gps, NULL, GET_GPS, NULL) != 0) {
        perror("pthread_create tid_gps failed");
    }
    sleep(1);

    int ret = dev_init();
    if (-1 == ret) {
        printf("Init communicate device error start reboot!\n");
        /* 硬件清理：关闭LED、关闭12V电源、同步RTC */
        system("echo 0 > /sys/class/leds/red/brightness");
        system("echo 0 > /sys/class/leds/yellow/brightness");
        system("echo 0 > /sys/class/leds/green/brightness");
        system("/home/root/power_12v.sh off");
        system("hwclock -w");
        system("killall wpa_supplicant");
        sleep(1);
        system("killall udhcpc");
        write_pid_file(watchPathName, "0");
        cout << "进程退出" << endl;
        /* g_CsoftwareWdt is now a stack object, no delete needed */
        return 0;
    }

    cout << "Init communicate device success" << endl;
    sleep(1);
    get_mac();
    cpu_occupy = get_cpuOccupy();
    cout << "CPU Usage Rate：" << cpu_occupy << endl;
    vector<vector<int>> vec;
    vec = CM->getALLIfd();
    deviceRegist registObj(g_CsoftwareWdt, &vec);
    if (pthread_create(&tid_deviceRegist, NULL, device_regist, &registObj) != 0) {
        perror("pthread_create tid_deviceRegist failed");
        tid_deviceRegist = 0;
    }
    if (pthread_create(&tid_softwd, NULL, softwarewd, g_CsoftwareWdt) != 0) {
        perror("pthread_create tid_softwd failed");
        tid_softwd = 0;
    }
    void* regRet;
    if (tid_deviceRegist != 0 && pthread_join(tid_deviceRegist, &regRet) == 0)
    {
        if (regRet == (void*)-1) {
            cout << "未注册成功" << endl;
            sleep(30);
            write_pid_file(watchPathName, "0");
            /* g_CsoftwareWdt is now a stack object, no delete needed */
            return 0;
        }
    }
    cout << "<<<<<<<<<<<<<<<<<<<<<Device Registration Successful<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
    int numRead = readFromFile(readMessage);
    cout << "读取到未发送的数据包数：" << numRead << endl;
    sleep(1);
    if (pthread_create(&tid_ship_data, NULL, get_ship_data, g_CsoftwareWdt) != 0) {
        perror("pthread_create tid_ship_data failed");
        tid_ship_data = 0;
    }
    if (pthread_create(&tid_transMessage, NULL, send_mess, g_CsoftwareWdt) != 0) {
        perror("pthread_create tid_transMessage failed");
        tid_transMessage = 0;
    }

    /* 看门狗线程设为分离态，不再 join（它永远不会退出） */
    if (tid_softwd != 0) {
        pthread_detach(tid_softwd);
    }

    /* 主线程等待信号，信号处理函数负责持久化数据和清理 */
    cout << "主线程等待信号退出..." << endl;
    pause();

    /* 不会到达此处，信号处理函数中调用 _exit(0) */
    return 0;
}
