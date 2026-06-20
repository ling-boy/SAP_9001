/**
 * @file message_transmitter.cpp
 * @brief 消息传输模块实现
 * @details 提供消息发送线程，监听通信设备接收数据并转发到网关
 */
#include "app/message_transmitter.h"
#include "infra/software_wdt.h"
#include "infra/communica_manage.h"
#include "infra/message_queue.h"
#include "infra/logger.h"
#include "protocol/protocol_process.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <string.h>
#include <errno.h>

/**
 * @brief 完整写入，处理 partial write
 * @return 成功写入全部数据返回 0，失败返回 -1
 */
static int write_full(int fd, const char* buf, size_t len)
{
    size_t total = 0;
    while (total < len) {
        ssize_t n = write(fd, buf + total, len - total);
        if (n <= 0) return -1;
        total += n;
    }
    return 0;
}

/* 协议标识定义 */
#define REQ_DATA    "03"
#define TIME_SEND   "20"
#define RX_SIZE     58

/* 外部全局变量（在 modbus_9001.cpp 中定义） */
extern MessageQueue<std::string> transMessage;
extern std::queue<std::string> readMessage;
extern pthread_mutex_t mtx_sensor;
extern pthread_cond_t cond_sensor;
extern int iS_getsensor;
extern int iS_getshipsensor;
extern int wdtNewSensorId;
extern int wdtShipId;
extern std::string mac;
extern unsigned int hj_crc;
extern communicaManage* CM;
extern const int softdogTimeout;

void* send_mess(void* arg)
{
    LOG_INFO("transmit", "start trans message to getway");
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
    while (1)
    {
        if (!CM) { sleep(1); continue; }
        int deviceFd = CM->getSuccessFd();
        LOG_INFO("transmit", "Enable FD Num：%d", deviceFd);
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
                    LOG_ERROR("transmit", "<<<<<<<<<<<<<ISR Quit<<<<<<<<<<<<<<");
                    LOG_ERROR("transmit", "<<<<<<<<<<<<<SAP Will reConnect<<<<<<<<<<<<<<");
                    return 0;
                }
                {
                    std::string recv_message(RX_buf, RX_buf + ret);
                    LOG_INFO("transmit", "Message From The Gateway：%s", recv_message.c_str());
                    if (recv_message.length() < 38) continue;
                    std::string recv_protocal = recv_message.substr(1, 2);
                    if (recv_protocal == REQ_DATA)
                    {
                        std::string recv_mac = recv_message.substr(22, 16);
                        if (recv_mac == mac)
                        {
                            count++;
                            LOG_INFO("transmit", "当前消息报计数器: %d", count);
                            g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
                            LOG_INFO("transmit", "thread of trans_message feed dog success");
                            pthread_mutex_lock(&mtx_sensor);
                            iS_getsensor = 1;
                            iS_getshipsensor = 1;
                            pthread_mutex_unlock(&mtx_sensor);
                            pthread_cond_broadcast(&cond_sensor);
                            pthread_mutex_lock(&mtx_sensor);
                            while (!readMessage.empty()) {
                                flag = false;
                                LOG_INFO("transmit", "当前未发送的缓存队列还剩余： %zu", readMessage.size());
                                std::string str = readMessage.front();
                                readMessage.pop();
                                pthread_mutex_unlock(&mtx_sensor);
                                ret = write_full(deviceFd, str.c_str(), str.size());
                                if (ret < 0)
                                {
                                    LOG_ERROR("transmit", "TransFd = %d", deviceFd);
                                    LOG_ERROR("transmit", "data_packet = %s", str.c_str());
                                    LOG_ERROR("transmit", "Trans error(deviceFd): %s", strerror(errno));
                                    pthread_mutex_lock(&mtx_sensor);
                                    readMessage.push(str);
                                    pthread_mutex_unlock(&mtx_sensor);
                                }
                                else
                                {
                                    sleep(2);
                                    g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
                                    LOG_INFO("transmit", "发送离线缓存包");
                                    LOG_INFO("transmit", "hj_crc: %u", hj_crc);
                                    LOG_INFO("transmit", "=======================");
                                    LOG_INFO("transmit", "data_packet = %s", str.c_str());
                                    LOG_INFO("transmit", "Trans success(deviceFd) %d", deviceFd);
                                    LOG_INFO("transmit", "包的长度 %zu", strlen(str.c_str()));
                                }
                                g_CsoftwareWdt->KeepSoftwareWdtAlive(wdtNewSensorId);
                                g_CsoftwareWdt->KeepSoftwareWdtAlive(wdtShipId);
                                pthread_mutex_lock(&mtx_sensor);
                            }
                            pthread_mutex_unlock(&mtx_sensor);
                            if (flag == false) {
                                flag = true;
                                continue;
                            }
                            std::string str;
                            if (transMessage.tryPop(str)) {
                                LOG_INFO("transmit", "当前缓存队列剩余： %zu", transMessage.size());
                                ret = write_full(deviceFd, str.c_str(), str.size());
                                if (ret < 0)
                                {
                                    LOG_ERROR("transmit", "TransFd = %d", deviceFd);
                                    LOG_ERROR("transmit", "data_packet = %s", str.c_str());
                                    LOG_ERROR("transmit", "Trans error(deviceFd): %s", strerror(errno));
                                    transMessage.push(str);
                                    continue;
                                }
                                else
                                {
                                    LOG_INFO("transmit", "hj_crc: %u", hj_crc);
                                    LOG_INFO("transmit", "=======================");
                                    LOG_INFO("transmit", "data_packet = %s", str.c_str());
                                    LOG_INFO("transmit", "Trans success(deviceFd) %d", deviceFd);
                                    LOG_INFO("transmit", "包的长度 %zu", strlen(str.c_str()));
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
