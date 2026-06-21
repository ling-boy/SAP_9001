#include "app/regist_manage.h"
#include "hal/gps.h"
#include "infra/logger.h"
#include "infra/io_utils.h"
#include "infra/retry_policy.h"
#include "infra/config.h"
#include "core/device_context.h"
#include <cstdlib>
#include <sys/time.h>
#include <time.h>

// 使用公共的 write_full 函数
using sap::write_full;

/**
 * @brief 硬件清理：关闭LED、关闭12V电源、同步RTC
 * @warning **命令注入风险**：本函数使用 system() 执行 shell 命令。
 *          当前所有命令均为硬编码常量，不存在注入风险。
 *          若未来引入外部输入拼接命令，需改用 execve() 等安全接口。
 *          参考：CWE-78 (OS Command Injection)
 */
static void hardware_cleanup()
{
    system("echo 0 > /sys/class/leds/red/brightness");
    system("echo 0 > /sys/class/leds/yellow/brightness");
    system("echo 0 > /sys/class/leds/green/brightness");
    std::string power_off = CFG_STR("paths", "power_script", "./power_12v.sh") + " off";
    system(power_off.c_str());
    system("hwclock -w");
    system("killall wpa_supplicant");
    sleep(1);
    system("killall udhcpc");
}

/**
 * @brief 注册状态枚举
 */
enum class RegistState {
    SelectChannel,      // 选择信道
    WaitForIdle,        // 等待信道空闲
    SendRegister,       // 发送注册请求
    WaitFor17,          // 等待17协议参数
    SendConfirm,        // 发送02确认包
    WaitForTimestamp,   // 等待20协议时间戳
    Success,            // 注册成功
    Failed              // 注册失败
};

/**
 * @brief 设备注册线程，通过select监听通信设备，发送注册请求，等待ISR下发17协议参数和20协议时间戳
 * @param arg 设备注册结构体指针（deviceRegist），包含软件看门狗、通信设备向量等资源
 * @return 成功返回(void*)0，失败返回(void*)-1
 */
void* device_regist(void* arg)
{
    auto& ctx = sap::DeviceContext::instance();
    LOG_INFO("regist", "%s", "device_regist start!!!");

    struct deviceRegist* dev = (struct deviceRegist*)arg;
    CSoftwareWdt* g_CsoftwareWdt = dev->g_CsoftwareWdt;
    std::vector<std::vector<int>>* vec = dev->vec;

    if ((*vec).empty()) {
        LOG_ERROR("regist", "%s", "device_regist: no communication devices available");
        return (void*)-1;
    }

    // 初始化状态机变量
    int size = static_cast<int>((*vec).size()) - 1;
    int regist_num = 1;
    int select_num = 1;
    int myCount = 0;
    int listen_num = 8;
    int fd = -1;
    int device_id = -1;
    int ret;
    uint8_t RX_buf[RX_SIZE];
    fd_set fdset;
    struct timeval tv;
    std::string regist_message;

    // 注册看门狗
    const char* const threadname = "device_regist";
    const int softdogTimeout = 60;
    int wdt_id = g_CsoftwareWdt->RequestSoftwareWdtID(threadname, softdogTimeout);
    g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);

    // 指数退避：设备切换时使用，基础5秒，最大60秒
    sap::ExponentialBackoff device_backoff(5000, 60000);

    // 状态机主循环
    RegistState state = RegistState::SelectChannel;

    while (state != RegistState::Success && state != RegistState::Failed) {
        switch (state) {

        case RegistState::SelectChannel:
            // 当前通信设备已重试5次，切换到下一个通信设备
            if (regist_num == 5) {
                size--;
                regist_num = 1;
                int delay = device_backoff.nextDelay();
                LOG_INFO("regist", "Switching to next comm device, backoff delay=%dms", delay);
                usleep(delay * 1000);
            }
            // 一轮注册完毕（所有通信设备均尝试失败），退出注册线程
            if (size < 0) {
                hardware_cleanup();
                LOG_ERROR("regist", "%s", "all devices tried, exiting registration thread");
                state = RegistState::Failed;
                break;
            }
            // 准备当前设备的 fd 和 ID
            fd = (*vec)[size][1];
            device_id = (*vec)[size][0];
            select_num = 1;
            myCount = 0;
            state = RegistState::WaitForIdle;
            break;

        case RegistState::WaitForIdle:
            while (true) {
                FD_ZERO(&fdset);
                FD_SET(fd, &fdset);
                tv.tv_sec = 15;
                tv.tv_usec = 0;
                myCount++;
                if (myCount >= 100) {
                    state = RegistState::Failed;
                    break;
                }
                g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);

                ret = select(fd + 1, &fdset, NULL, NULL, &tv);
                if (ret > 0) {
                    if (FD_ISSET(fd, &fdset)) {
                        memset(RX_buf, 0, sizeof(RX_buf));
                        ret = read(fd, RX_buf, RX_SIZE);
                        if (ret <= 0) {
                            LOG_ERROR("regist", "regist read error or EOF, ret=%d", ret);
                            continue;
                        }
                        // 该包为最后一个数据包，信道空闲
                        if (ret >= 2 && ret < 58 && RX_buf[0] != '$' && RX_buf[ret - 1] == '@') {
                            g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
                            state = RegistState::SendRegister;
                            break;
                        }
                        // 连续收到4个完整数据包后 select_num 达到 5，判定信道空闲
                        if (RX_buf[0] == '$' && RX_buf[ret - 1] == '@' && select_num == 5) {
                            g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
                            state = RegistState::SendRegister;
                            break;
                        }
                        select_num++;
                    }
                }
                // select超时（15秒），表示信道空闲
                else if (ret == 0) {
                    g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
                    state = RegistState::SendRegister;
                    break;
                }
                // select错误
                else {
                    state = RegistState::Failed;
                    break;
                }
            }
            break;

        case RegistState::SendRegister:
            sleep(ctx.getIdentityMonitorTime() / 4);
            {
                // 动态计算数据段长度
                std::string data_part = ctx.getIdentityMac() + ctx.getCommunicateStatus() +
                    "010" + gps_get_location() + ctx.getIdentityCpuOccupy() + "@";
                int length = data_part.length() + 14;  // 固定字段：$01 + device_id + id + net_id + 00 = 14字节
                char len_buf[5];
                snprintf(len_buf, sizeof(len_buf), "%04X", length);

                regist_message = std::string("$") + "01" + std::to_string(device_id) +
                    ctx.getIdentityId() + ctx.getIdentityNetId() + "00" + len_buf +
                    data_part;
            }

            ret = write_full(fd, regist_message.c_str(), regist_message.size());
            if (ret < 0) {
                LOG_ERROR("regist", "device_regist: write error: %s", strerror(errno));
                select_num = 1;
                regist_num++;
                state = RegistState::SelectChannel;
            } else {
                LOG_INFO("regist", "Regist: %s", regist_message.c_str());
                state = RegistState::WaitFor17;
            }
            break;

        case RegistState::WaitFor17: {
            int retry = 5;
            bool received_17 = false;
            while (retry-- > 0 && !received_17) {
                FD_ZERO(&fdset);
                FD_SET(fd, &fdset);
                tv.tv_sec = ctx.getIdentityMonitorTime();
                tv.tv_usec = 0;
                g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);

                ret = select(fd + 1, &fdset, NULL, NULL, &tv);
                if (ret > 0) {
                    g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
                    memset(RX_buf, 0, sizeof(RX_buf));
                    ret = read(fd, RX_buf, RX_SIZE);
                    if (ret <= 0) {
                        LOG_ERROR("regist", "regist read error or EOF, ret=%d", ret);
                        continue;
                    }
                    std::string regist_recv_message(RX_buf, RX_buf + ret);
                    if (regist_recv_message.length() < 54) {
                        LOG_WARN("regist", "regist_recv_message too short: %zu", regist_recv_message.length());
                        continue;
                    }
                    std::string recv_protocol = regist_recv_message.substr(1, 2);
                    // 收到17协议，提取ISR的MAC地址、设备ID和网络ID
                    // 注意：此处假设17协议格式为固定偏移，需与ISR端协议文档对照确认
                    if (recv_protocol == REQ_SEND_INFO) {
                        std::string recv_mac = regist_recv_message.substr(16, 16);
                        if (recv_mac == ctx.getIdentityMac()) {
                            ctx.setIdentityIsrMac(regist_recv_message.substr(32, 16));
                            ctx.setIdentityId(regist_recv_message.substr(48, 2));
                            ctx.setIdentityNetId(regist_recv_message.substr(50, 4));
                            LOG_INFO("regist", "Gateway assigned ID: %s", ctx.getIdentityId().c_str());
                            received_17 = true;
                        }
                    }
                }
            }
            if (received_17 && ctx.getIdentityId() != "FF") {
                state = RegistState::SendConfirm;
            } else if (ctx.getIdentityId() == "FF") {
                // 注册被拒绝，重试
                select_num = 1;
                regist_num++;
                state = RegistState::SelectChannel;
            } else {
                // 等待超时，重试
                select_num = 1;
                regist_num++;
                state = RegistState::SelectChannel;
            }
            break;
        }

        case RegistState::SendConfirm: {
            std::string confirm_message = std::string("$") + "02" + std::to_string(device_id) +
                ctx.getIdentityId() + ctx.getIdentityNetId() + "00" + "0012" +
                ctx.getIdentityMac() + ctx.getIdentityId() + "@";
            ret = write_full(fd, confirm_message.c_str(), confirm_message.size());
            if (ret < 0) {
                LOG_ERROR("regist", "%s", "write confirm_message failed!");
            }
            LOG_INFO("regist", "confirm_message: %s", confirm_message.c_str());
            listen_num = 8;
            state = RegistState::WaitForTimestamp;
            break;
        }

        case RegistState::WaitForTimestamp: {
            bool timestamp_received = false;
            while (listen_num > 0 && !timestamp_received) {
                g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
                FD_ZERO(&fdset);
                FD_SET(fd, &fdset);
                tv.tv_sec = ctx.getIdentityMonitorTime();
                tv.tv_usec = 0;
                ret = select(fd + 1, &fdset, NULL, NULL, &tv);
                listen_num--;
                LOG_INFO("regist", "Timestamp listen counter: %d", listen_num);

                if (ret > 0) {
                    memset(RX_buf, 0, sizeof(RX_buf));
                    ret = read(fd, RX_buf, RX_SIZE);
                    if (ret <= 0) {
                        LOG_ERROR("regist", "ISR QUIT or read error, ret=%d", ret);
                        continue;
                    }
                    std::string time_recvmessage(RX_buf, RX_buf + ret);
                    if (time_recvmessage.length() < 49) {
                        LOG_WARN("regist", "time_recvmessage too short: %zu", time_recvmessage.length());
                        continue;
                    }
                    std::string recv_protocol = time_recvmessage.substr(1, 2);
                    std::string recv_mac = time_recvmessage.substr(16, 16);
                    // 收到20协议时间戳，设置系统时间并使能设备
                    if (recv_protocol == TIME_SEND && recv_mac == ctx.getIdentityMac()) {
                        ctx.setIdentityCurrentTime(time_recvmessage.substr(32, 17));
                        struct tm tm_set;
                        memset(&tm_set, 0, sizeof(tm_set));
                        std::string current_time = ctx.getIdentityCurrentTime();
                        tm_set.tm_year = atoi(current_time.substr(0, 4).c_str()) - 1900;
                        tm_set.tm_mon  = atoi(current_time.substr(4, 2).c_str()) - 1;
                        tm_set.tm_mday = atoi(current_time.substr(6, 2).c_str());
                        tm_set.tm_hour = atoi(current_time.substr(8, 2).c_str());
                        tm_set.tm_min  = atoi(current_time.substr(10, 2).c_str());
                        tm_set.tm_sec  = atoi(current_time.substr(12, 2).c_str());
                        struct timeval tv_set;
                        tv_set.tv_sec = mktime(&tm_set);
                        if (tv_set.tv_sec == (time_t)-1) {
                            LOG_ERROR("regist", "%s", "mktime failed, invalid timestamp");
                            continue;
                        }
                        tv_set.tv_usec = atoi(current_time.substr(14, 3).c_str()) * 1000;
                        if (settimeofday(&tv_set, NULL) != 0) {
                            LOG_ERROR("regist", "settimeofday failed: %s", strerror(errno));
                        }
                        LOG_INFO("regist", "Set system time success: %s", current_time.c_str());
                        // 设置系统时间后，将当前通信设备设置为使能状态
                        if (ctx.commManager().setEnable(device_id)) {
                            LOG_INFO("regist", "Device ID=%d enabled", device_id);
                            LOG_INFO("regist", "%s", "SAP register success");
                            timestamp_received = true;
                            state = RegistState::Success;
                        }
                    }
                }
            }
            // 等待时间戳超时（监听计数耗尽），重试发送确认包
            if (!timestamp_received && listen_num == 0) {
                LOG_WARN("regist", "%s", "Timestamp not received, retrying confirm...");
                regist_num++;
                if (regist_num >= 5) {
                    LOG_ERROR("regist", "%s", "Timestamp retry exhausted, registration failed");
                    hardware_cleanup();
                    state = RegistState::Failed;
                } else {
                    listen_num = 8;
                    state = RegistState::SendConfirm;
                }
            }
            break;
        }

        default:
            state = RegistState::Failed;
            break;
        }
    }

    // 释放看门狗
    g_CsoftwareWdt->ReleaseSoftwareWdtID(threadname, wdt_id);

    if (state == RegistState::Success) {
        return (void*)0;
    }
    return (void*)-1;
}
