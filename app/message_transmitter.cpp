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
#define RX_SIZE     58

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
    int flag = true;
    int heartbeat_counter = 0;
    const int HEARTBEAT_INTERVAL = 2;  // 每2次超时（约60秒）发送一次心跳
    while (1)
    {
        int deviceFd = ctx.commManager().getSuccessFd();
        LOG_INFO("transmit", "Enable FD Num: %d", deviceFd);
        if (deviceFd < 0) {
            sleep(1);
            continue;
        }
        FD_ZERO(&fdset);
        FD_SET(deviceFd, &fdset);
        tv.tv_sec = 30;
        tv.tv_usec = 0;
        ret = select(deviceFd + 1, &fdset, NULL, NULL, &tv);
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
                    return 0;
                }
                {
                    std::string recv_message(RX_buf, RX_buf + ret);
                    LOG_INFO("transmit", "Message From The Gateway: %s", recv_message.c_str());
                    if (recv_message.length() < 38) continue;
                    std::string recv_protocal = recv_message.substr(1, 2);
                    if (recv_protocal == REQ_DATA)
                    {
                        std::string recv_mac = recv_message.substr(22, 16);
                        if (recv_mac == ctx.identity().mac)
                        {
                            count++;
                            LOG_INFO("transmit", "Message counter: %d", count);
                            g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
                            LOG_INFO("transmit", "%s", "thread of trans_message feed dog success");
                            pthread_mutex_lock(&ctx.sensorSync().mtx);
                            ctx.sensorSync().gas_data_ready = 1;
                            ctx.sensorSync().ship_data_ready = 1;
                            pthread_mutex_unlock(&ctx.sensorSync().mtx);
                            pthread_cond_broadcast(&ctx.sensorSync().cond);
                            pthread_mutex_lock(&ctx.sensorSync().mtx);
                            while (!ctx.queues().offline_cache.empty()) {
                                flag = false;
                                LOG_INFO("transmit", "Offline cache remaining: %zu", ctx.queues().offline_cache.size());
                                std::string str = ctx.queues().offline_cache.front();
                                ctx.queues().offline_cache.pop();
                                pthread_mutex_unlock(&ctx.sensorSync().mtx);
                                ret = write_full(deviceFd, str.c_str(), str.size());
                                if (ret < 0)
                                {
                                    LOG_ERROR("transmit", "TransFd = %d", deviceFd);
                                    LOG_ERROR("transmit", "data_packet = %s", str.c_str());
                                    LOG_ERROR("transmit", "Trans error(deviceFd): %s", strerror(errno));
                                    pthread_mutex_lock(&ctx.sensorSync().mtx);
                                    ctx.queues().offline_cache.push(str);
                                    pthread_mutex_unlock(&ctx.sensorSync().mtx);
                                }
                                else
                                {
                                    sleep(2);
                                    g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
                                    LOG_INFO("transmit", "%s", "Sent offline cache packet");
                                    LOG_INFO("transmit", "hj_crc: %u", ctx.queues().hj_crc);
                                    LOG_INFO("transmit", "%s", "=======================");
                                    LOG_INFO("transmit", "data_packet = %s", str.c_str());
                                    LOG_INFO("transmit", "Trans success(deviceFd) %d", deviceFd);
                                    LOG_INFO("transmit", "Packet length %zu", strlen(str.c_str()));
                                }
                                g_CsoftwareWdt->KeepSoftwareWdtAlive(ctx.watchdog().gas_wdt_id);
                                g_CsoftwareWdt->KeepSoftwareWdtAlive(ctx.watchdog().ship_wdt_id);
                                pthread_mutex_lock(&ctx.sensorSync().mtx);
                            }
                            pthread_mutex_unlock(&ctx.sensorSync().mtx);
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
