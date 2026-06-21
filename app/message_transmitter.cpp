/**
 * @file message_transmitter.cpp
 * @brief 消息传输模块实现
 * @details 提供消息发送线程，监听通信设备接收数据并转发到网关
 */
#include "app/message_transmitter.h"
#include "infra/software_wdt.h"
#include "infra/communica_manage.h"
#include "infra/message_queue.h"
#include "infra/io_utils.h"
#include "infra/logger.h"
#include "infra/retry_policy.h"
#include "protocol/protocol_process.h"
#include "core/device_context.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <string.h>
#include <errno.h>

// 使用公共的 write_full 函数
using sap::write_full;

// 使用统一的协议常量定义
#include "protocol/constants.h"

/* 所有全局状态已迁移至 DeviceContext 单例 */
extern const int softdogTimeout;

void* send_mess(void* arg)
{
    auto& ctx = sap::DeviceContext::instance();
    LOG_INFO("transmit", "%s", "start trans message to gateway");
    uint8_t RX_buf[RX_SIZE];
    int ret = -1;
    fd_set fdset;
    FD_ZERO(&fdset);
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    CSoftwareWdt* g_CsoftwareWdt = (CSoftwareWdt*)arg;
    int count = 0;
    const char* const threadname = "trans_message";
    int wdt_id = -1;
    wdt_id = g_CsoftwareWdt->RequestSoftwareWdtID(threadname, softdogTimeout);
    bool flag = true;
    int heartbeat_counter = 0;
    const int HEARTBEAT_INTERVAL = 2;  // 每2次超时（约60秒）发送一次心跳

    // 指数退避：FD获取失败时使用，基础1秒，最大30秒
    sap::ExponentialBackoff fd_backoff(1000, 30000);

    while (1)
    {
        int deviceFd = ctx.commManager().getSuccessFd();
        LOG_INFO("transmit", "Enable FD Num: %d", deviceFd);
        if (deviceFd < 0) {
            int delay = fd_backoff.nextDelay();
            LOG_WARN("transmit", "No available FD, backoff delay=%dms", delay);
            usleep(delay * 1000);
            continue;
        }
        // 成功获取FD，重置退避
        fd_backoff.reset();
        FD_ZERO(&fdset);
        FD_SET(deviceFd, &fdset);
        tv.tv_sec = 30;
        tv.tv_usec = 0;
        ret = select(deviceFd + 1, &fdset, NULL, NULL, &tv);

        // 检查 fd 是否仍然有效（防止 reinit 关闭了 fd）
        int currentFd = ctx.commManager().getSuccessFd();
        if (currentFd != deviceFd) {
            LOG_WARN("transmit", "FD changed during select (was %d, now %d), retrying", deviceFd, currentFd);
            continue;
        }

        if (ret == 0) {
            /* select 超时，喂狗后继续循环 */
            g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);

            // 发送心跳包
            heartbeat_counter++;
            if (heartbeat_counter >= HEARTBEAT_INTERVAL) {
                heartbeat_counter = 0;
                std::string heartbeat = buildHeartbeat();
                ret = write_full(deviceFd, heartbeat.c_str(), heartbeat.size());
                if (ret < 0) {
                    LOG_WARN("transmit", "Heartbeat send failed: %s", strerror(errno));
                } else {
                    LOG_INFO("transmit", "Heartbeat sent: %s", heartbeat.c_str());
                }
            }
            continue;
        }
        if (ret < 0) {
            LOG_ERROR("transmit", "select error in send_mess: %s", strerror(errno));
            sleep(1);
            continue;
        }
        if (ret > 0)
        {
            if (FD_ISSET(deviceFd, &fdset))
            {
                memset(RX_buf, 0, sizeof(RX_buf));
                ret = read(deviceFd, RX_buf, RX_SIZE);
                if (ret < 0) {
                    LOG_ERROR("transmit", "read error in send_mess: %s", strerror(errno));
                    continue;
                }
                if (ret == 0) {
                    LOG_ERROR("transmit", "%s", "<<<<<<<<<<<<<ISR Quit<<<<<<<<<<<<<<");
                    LOG_ERROR("transmit", "%s", "<<<<<<<<<<<<<SAP Will reConnect<<<<<<<<<<<<<<");
                    // 触发重连，使用指数退避避免熔断后空转
                    int activeId = ctx.commManager().getSuccessId();
                    if (activeId >= 0) {
                        if (!ctx.commManager().reinit(activeId, 0)) {
                            // 熔断器打开，等待冷却时间后再重试
                            LOG_WARN("transmit", "%s", "Circuit breaker open, waiting 30s before retry");
                            sleep(30);
                        }
                    }
                    sleep(2);
                    continue;
                }
                {
                    std::string recv_message(RX_buf, RX_buf + ret);
                    LOG_INFO("transmit", "Message From The Gateway: %s", recv_message.c_str());
                    if (recv_message.length() < 38) continue;
                    std::string recv_protocal = recv_message.substr(1, 2);
                    if (recv_protocal == REQ_DATA)
                    {
                        std::string recv_mac = recv_message.substr(22, 16);
                        if (recv_mac == ctx.getIdentityMac())
                        {
                            count++;
                            LOG_INFO("transmit", "Message counter: %d", count);
                            g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
                            LOG_INFO("transmit", "%s", "thread of trans_message feed dog success");
                            ctx.notifySensorDataReady(true);   /* ship_data_ready = 1 */
                            ctx.notifySensorDataReady(false);  /* gas_data_ready = 1 */

                            // 先取后发模式：在锁内取出所有数据，解锁后逐个发送
                            std::vector<std::string> pending_packets;
                            std::vector<std::string> failed_packets;
                            pthread_mutex_lock(&ctx.queues().offline_mtx);
                            while (!ctx.queues().offline_cache.empty()) {
                                pending_packets.push_back(ctx.queues().offline_cache.front());
                                ctx.queues().offline_cache.pop();
                            }
                            pthread_mutex_unlock(&ctx.queues().offline_mtx);

                            if (!pending_packets.empty()) {
                                flag = false;
                                for (auto& str : pending_packets) {
                                    LOG_INFO("transmit", "Sending offline cache packet: %s", str.c_str());
                                    ret = write_full(deviceFd, str.c_str(), str.size());
                                    if (ret < 0) {
                                        LOG_ERROR("transmit", "Trans error(deviceFd): %s", strerror(errno));
                                        failed_packets.push_back(str);
                                    } else {
                                        sleep(2);
                                        g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
                                        LOG_INFO("transmit", "Sent offline cache packet success");
                                    }
                                    // 只在传感器线程启动后才喂狗（ID >= 0）
                                    if (ctx.watchdog().gas_wdt_id >= 0) {
                                        g_CsoftwareWdt->KeepSoftwareWdtAlive(ctx.watchdog().gas_wdt_id);
                                    }
                                    if (ctx.watchdog().ship_wdt_id >= 0) {
                                        g_CsoftwareWdt->KeepSoftwareWdtAlive(ctx.watchdog().ship_wdt_id);
                                    }
                                }
                                // 将发送失败的数据统一放回队列
                                if (!failed_packets.empty()) {
                                    pthread_mutex_lock(&ctx.queues().offline_mtx);
                                    for (auto& str : failed_packets) {
                                        ctx.queues().offline_cache.push(str);
                                    }
                                    pthread_mutex_unlock(&ctx.queues().offline_mtx);
                                }
                            }
                            if (flag == false) {
                                flag = true;
                                continue;
                            }
                            std::string str;
                            if (ctx.queues().transmit.tryPop(str)) {
                                LOG_INFO("transmit", "Transmit queue remaining: %zu", ctx.queues().transmit.size());
                                ret = write_full(deviceFd, str.c_str(), str.size());
                                if (ret < 0)
                                {
                                    LOG_ERROR("transmit", "TransFd = %d", deviceFd);
                                    LOG_ERROR("transmit", "data_packet = %s", str.c_str());
                                    LOG_ERROR("transmit", "Trans error(deviceFd): %s", strerror(errno));
                                    ctx.queues().transmit.push(str);
                                    continue;
                                }
                                else
                                {
                                    LOG_INFO("transmit", "hj_crc: %u", ctx.queues().hj_crc);
                                    LOG_INFO("transmit", "%s", "=======================");
                                    LOG_INFO("transmit", "data_packet = %s", str.c_str());
                                    LOG_INFO("transmit", "Trans success(deviceFd) %d", deviceFd);
                                    LOG_INFO("transmit", "Packet length %zu", strlen(str.c_str()));
                                    continue;
                                }
                            }
                        }
                    }
                    else if (recv_protocal == TIME_SEND) {
                        count = 0;
                        g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
                        packet20(recv_message);
                    }
                }
            }
        }
    }
}
