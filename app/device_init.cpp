/**
 * @file device_init.cpp
 * @brief 设备初始化模块实现
 * @details 提供通信设备初始化、重初始化、MAC读取等功能
 */
#include "app/device_init.h"
#include "hal/lora.h"
#include "hal/bluetooth.h"
#include "infra/communica_manage.h"
#include "infra/config.h"
#include "core/device_context.h"
#include "core/communication_factory.h"
#include "hal/bt_strategy.h"
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
// logger.h must be included AFTER all other headers to override syslog.h LOG_INFO
#include "infra/logger.h"

/* 所有全局状态已迁移至 DeviceContext 单例 */

/* 通信设备管理ID号 */
#define LORA 1
#define WIFI 2
#define BT   3
#define LAN  4

int connect_nonb(int sockfd, const struct sockaddr* saptr, socklen_t salen, int nsec)
{
    int     flags, n, error;
    socklen_t   len;
    fd_set  rset, wset;
    struct timeval  tval;

    if ((flags = fcntl(sockfd, F_GETFL, 0)) == -1) {
        LOG_ERROR("dev_init", "fcntl F_GETFL: %s", strerror(errno));
        close(sockfd);
        return -1;
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        LOG_ERROR("dev_init", "fcntl F_SETFL: %s", strerror(errno));
        close(sockfd);
        return -1;
    }

    error = 0;
    if ((n = connect(sockfd, saptr, salen)) < 0) {
        if (errno != EINPROGRESS) {
            fcntl(sockfd, F_SETFL, flags);
            close(sockfd);
            return -1;
        }
    }
    else if (n == 0) {
        goto done;
    }

    FD_ZERO(&rset);
    FD_SET(sockfd, &rset);
    wset = rset;
    tval.tv_sec = nsec;
    tval.tv_usec = 0;

    if ((n = select(sockfd + 1, &rset, &wset, NULL, nsec ? &tval : NULL)) == 0) {
        errno = ETIMEDOUT;
        fcntl(sockfd, F_SETFL, flags);  // 恢复 socket flags
        close(sockfd);
        return -1;
    }
    else if (n == -1) {
        LOG_ERROR("dev_init", "select: %s", strerror(errno));
        fcntl(sockfd, F_SETFL, flags);  // 恢复 socket flags
        close(sockfd);
        return -1;
    }

    if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
        len = sizeof(error);
        if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
            fcntl(sockfd, F_SETFL, flags);  // 恢复 socket flags
            close(sockfd);
            return -1;
        }
    }
    else {
        LOG_ERROR("dev_init", "%s", "select error: socket not set");
        fcntl(sockfd, F_SETFL, flags);  // 恢复 socket flags
        close(sockfd);
        return -1;
    }

done:
    if (fcntl(sockfd, F_SETFL, flags) == -1) {
        LOG_ERROR("dev_init", "fcntl: %s", strerror(errno));
    }

    if (error) {
        errno = error;
        close(sockfd);
        return -1;
    }

    return 0;
}

int lora_reinit(int old_fd)
{
    auto& ctx = sap::DeviceContext::instance();
    // 先打开新 fd，成功后再关闭旧 fd，避免 double-close
    int new_fd = lora_open();
    if (new_fd == -1)
    {
        LOG_ERROR("dev_init", "%s", "lora reinit failed");
        return -1;
    }
    // 成功后关闭旧 fd
    close(old_fd);
    ctx.fds().lora = new_fd;
    {
        ctx.setCommunicateStatus(0, '1');
        LOG_INFO("dev_init", "%s", "lora reinit success");
        return ctx.fds().lora;
    }
}

int wifi_reinit(int old_fd)
{
    auto& ctx = sap::DeviceContext::instance();
    close(old_fd);
    if (system("ifconfig wlan0 down") != 0)
        LOG_ERROR("dev_init", "%s", "wifi_reinit: ifconfig wlan0 down failed");
    sleep(2);
    if (system("killall wpa_supplicant") != 0)
        LOG_ERROR("dev_init", "%s", "wifi_reinit: killall wpa_supplicant failed");
    sleep(1);
    if (system("killall udhcpc") != 0)
        LOG_ERROR("dev_init", "%s", "wifi_reinit: killall udhcpc failed");
    sleep(1);
    std::string wifi_conf = CFG_STR("paths", "wifi_conf", "/home/root/wifi.conf");
    std::string wpa_cmd = "wpa_supplicant -i wlan0 -B -c " + wifi_conf + " &";
    if (system(wpa_cmd.c_str()) != 0)
        LOG_ERROR("dev_init", "%s", "wifi_reinit: wpa_supplicant start failed");
    sleep(10);
    if (system("udhcpc -i wlan0 -t 8 -n") != 0)
        LOG_ERROR("dev_init", "%s", "wifi_reinit: udhcpc failed");
    int fd_wifi_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_wifi_sock < 0)
    {
        LOG_ERROR("dev_init", "%s", "wifi: socket create failure");
        return -1;
    }
    else
    {
        LOG_INFO("dev_init", "%s", "wifi: socket create success");
        struct sockaddr_in wifi_ser_addr;
        memset(&wifi_ser_addr, 0x0, sizeof(struct sockaddr_in));
        wifi_ser_addr.sin_family = AF_INET;
        wifi_ser_addr.sin_port = htons(cfg_wifi_port());
        wifi_ser_addr.sin_addr.s_addr = inet_addr(cfg_wifi_ip().c_str());
        int wifi_fd_connect;
        wifi_fd_connect = connect_nonb(fd_wifi_sock, (struct sockaddr*)&wifi_ser_addr, sizeof(struct sockaddr_in), cfg_connect_timeout());
        if (wifi_fd_connect < 0)
        {
            LOG_ERROR("dev_init", "wifi: connect: %s", strerror(errno));
            ctx.fds().wifi = -1;
            system("killall wpa_supplicant");
            system("killall udhcpc");
            return -1;
        }
        else
        {
            LOG_INFO("dev_init", "%s", "wifi: connect server success");
            LOG_INFO("dev_init", "Init success: wifi. Device descriptor = %d", fd_wifi_sock);
            ctx.fds().wifi = fd_wifi_sock;
            ctx.setCommunicateStatus(1, '1');
            system("echo 1 > /sys/class/leds/yellow/brightness");
            return fd_wifi_sock;
        }
    }
}

int bt_reinit(int old_fd)
{
    auto& ctx = sap::DeviceContext::instance();
    close(old_fd);
    std::string bt_mac = ctx.getIdentityBtMac();
    ctx.fds().bt = bluetooth_open(bt_mac);
    if (-1 == ctx.fds().bt)
    {
        ctx.setIdentityBtMac("");
        LOG_ERROR("dev_init", "%s", "-> No blue_tooth device");
        return -1;
    }
    else
    {
        LOG_INFO("dev_init", "Init success: bluetooth. Device descriptor = %d", ctx.fds().bt);
        ctx.setCommunicateStatus(2, '1');  // BT id=3, index=2
        system("echo 1 > /sys/class/leds/green/brightness");
        return ctx.fds().bt;
    }
}

int lan_reinit(int old_fd)
{
    auto& ctx = sap::DeviceContext::instance();
    close(old_fd);
    std::string eth1_ip = CFG_STR("network.lan", "local_ip", "192.168.2.235");
    std::string ifconfig_cmd = "ifconfig eth1 " + eth1_ip + " netmask 255.255.255.0";
    system(ifconfig_cmd.c_str());
    sleep(2);
    ctx.fds().lan = socket(AF_INET, SOCK_STREAM, 0);
    if (ctx.fds().lan < 0)
    {
        LOG_ERROR("dev_init", "lan: socket create error: %s", strerror(errno));
        return -1;
    }
    else
    {
        struct sockaddr_in lan_ser_addr;
        memset(&lan_ser_addr, 0, sizeof(lan_ser_addr));
        lan_ser_addr.sin_family = AF_INET;
        lan_ser_addr.sin_addr.s_addr = inet_addr(cfg_lan_ip().c_str());
        lan_ser_addr.sin_port = htons(cfg_lan_port());
        LOG_INFO("dev_init", "Init success: Lan. Device descriptor = %d", ctx.fds().lan);
        // 使用非阻塞 connect 避免长时间阻塞
        int lan_fd_connect = connect_nonb(ctx.fds().lan, (struct sockaddr*)&lan_ser_addr, sizeof(lan_ser_addr), cfg_connect_timeout());
        if (lan_fd_connect < 0)
        {
            LOG_ERROR("dev_init", "lan: connect: %s", strerror(errno));
            return -1;
        }
        else
        {
            int lan_fd = ctx.fds().lan;
            LOG_INFO("dev_init", "%s", "lanship: connect server success");
            LOG_INFO("dev_init", "Init success: Lan. Device descriptor = %d", lan_fd);
            ctx.setCommunicateStatus(3, '1');  // LAN id=4, index=3
            return lan_fd;
        }
    }
}

int dev_init()
{
    auto& ctx = sap::DeviceContext::instance();

    // 使用 CommunicationFactory 创建所有通信策略
    auto strategies = sap::CommunicationFactory::createAll();

    for (auto& strategy : strategies) {
        int fd = strategy->initialize();
        if (fd < 0) {
            LOG_ERROR("dev_init", "Failed to initialize %s", strategy->typeName().c_str());
            continue;
        }

        int id = strategy->deviceId();

        // 客户端连接需要注册到 commManager（服务器监听 socket 不需要）
        if (strategy->isClient()) {
            ctx.commManager().addCommunicateNode(id, fd);
            // 注册重初始化回调（使用原有的 reinit 函数）
            switch (id) {
                case 1: ctx.commManager().callbackRgist(id, 0, lora_reinit); break;
                case 2: ctx.commManager().callbackRgist(id, 0, wifi_reinit); break;
                case 3: ctx.commManager().callbackRgist(id, 0, bt_reinit); break;
                case 4: ctx.commManager().callbackRgist(id, 0, lan_reinit); break;
            }
            ctx.fds().device_id.push_back(fd);

            // 仅客户端设备设置 communicate_status
            // 索引：LoRa=0, WiFi=1, BT=2, LAN=3
            int status_idx = id - 1;
            if (status_idx >= 0 && status_idx <= 3) {
                ctx.setCommunicateStatus(status_idx, '1');
            }
        }

        // 更新对应的 fd
        switch (id) {
            case 1: ctx.fds().lora = fd; break;
            case 2: ctx.fds().wifi = fd; ctx.fds().flag_wifi = 1; break;
            case 3: ctx.fds().bt = fd; break;
            case 4: ctx.fds().lan = fd; break;
            case 5: ctx.fds().lan_server = fd; break;
        }

        LOG_INFO("dev_init", "Init success: %s, fd=%d", strategy->typeName().c_str(), fd);
        sleep(2);
    }

    // 特殊处理：获取蓝牙 MAC 地址
    auto* btStrategy = dynamic_cast<sap::BluetoothStrategy*>(
        [&strategies]() -> sap::ICommunicationStrategy* {
            for (auto& s : strategies) {
                if (s->deviceId() == 3) return s.get();
            }
            return nullptr;
        }());
    if (btStrategy && btStrategy->isConnected()) {
        ctx.setIdentityBtMac(btStrategy->getDeviceMac());
    }

    if (ctx.fds().device_id.empty()) {
        return -1;
    }
    return (int)ctx.fds().device_id.size();
}

void get_mac()
{
    auto& ctx = sap::DeviceContext::instance();
    std::string mac_file = CFG_STR("data", "mac_file", "./mac.txt");
    FILE* fp = fopen(mac_file.c_str(), "r");
    if (fp == NULL)
    {
        LOG_ERROR("dev_init", "%s", "get mac failed");
        return;
    }
    char buf[256] = {0};
    if (fgets(buf, sizeof(buf), fp) != NULL) {
        std::string mac_addr(buf, strlen(buf));
        /* 去除末尾换行符 */
        if (!mac_addr.empty() && mac_addr.back() == '\n') {
            mac_addr.pop_back();
        }
        ctx.setIdentityMac(mac_addr);
    }
    fclose(fp);
    return;
}
