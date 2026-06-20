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
        return -1;
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        LOG_ERROR("dev_init", "fcntl F_SETFL: %s", strerror(errno));
        return -1;
    }

    error = 0;
    if ((n = connect(sockfd, saptr, salen)) < 0) {
        if (errno != EINPROGRESS) {
            fcntl(sockfd, F_SETFL, flags);
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
        return -1;
    }
    else if (n == -1) {
        LOG_ERROR("dev_init", "select: %s", strerror(errno));
        return -1;
    }

    if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
        len = sizeof(error);
        if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
            return -1;
        }
    }
    else {
        LOG_ERROR("dev_init", "%s", "select error: socket not set");
        return -1;
    }

done:
    if (fcntl(sockfd, F_SETFL, flags) == -1) {
        LOG_ERROR("dev_init", "fcntl: %s", strerror(errno));
    }

    if (error) {
        errno = error;
        return -1;
    }

    return 0;
}

int lora_reinit(int old_fd)
{
    auto& ctx = sap::DeviceContext::instance();
    close(old_fd);
    ctx.fds().lora = lora_open();
    if (ctx.fds().lora == -1)
    {
        LOG_ERROR("dev_init", "%s", "lora reinit failed");
        return -1;
    }
    else
    {
        ctx.identity().communicate_status[0] = '1';
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
            close(fd_wifi_sock);
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
            ctx.identity().communicate_status[1] = '1';
            system("echo 1 > /sys/class/leds/yellow/brightness");
            return fd_wifi_sock;
        }
    }
}

int bt_reinit(int old_fd)
{
    auto& ctx = sap::DeviceContext::instance();
    close(old_fd);
    ctx.fds().bt = bluetooth_open(ctx.identity().bt_mac);
    if (-1 == ctx.fds().bt)
    {
        ctx.identity().bt_mac = "";
        LOG_ERROR("dev_init", "%s", "-> No blue_tooth device");
        return -1;
    }
    else
    {
        LOG_INFO("dev_init", "Init success: bluetooth. Device descriptor = %d", ctx.fds().bt);
        ctx.identity().communicate_status[3] = '1';
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
        /* TODO: 此处使用阻塞connect，若对端不可达会长时间阻塞。建议后续改为 connect_nonb 非阻塞方式 */
        int lan_fd_connect = connect(ctx.fds().lan, (struct sockaddr*)&lan_ser_addr, sizeof(lan_ser_addr));
        if (lan_fd_connect < 0)
        {
            LOG_ERROR("dev_init", "lan: connect: %s", strerror(errno));
            close(ctx.fds().lan);
            return -1;
        }
        else
        {
            int lan_fd = ctx.fds().lan;
            LOG_INFO("dev_init", "%s", "lanship: connect server success");
            LOG_INFO("dev_init", "Init success: Lan. Device descriptor = %d", lan_fd);
            ctx.identity().communicate_status[2] = '1';
            return lan_fd;
        }
    }
}

int dev_init()
{
    auto& ctx = sap::DeviceContext::instance();
    ctx.fds().lora = lora_open();
    if (-1 == ctx.fds().lora)
    {
#ifdef DEBUG
        LOG_ERROR("dev_init", "%s", "--> No lora device");
#endif
    }
    else
    {
        int lora_fd = ctx.fds().lora;
        ctx.fds().device_id.push_back(lora_fd);
        ctx.commManager().addCommunicateNode(LORA, lora_fd);
        ctx.commManager().callbackRgist(LORA, 0, lora_reinit);
        LOG_INFO("dev_init", "Init success: lora. Device descriptor = %d", lora_fd);
        ctx.identity().communicate_status[0] = '1';
        system("echo 1 > /sys/class/leds/red/brightness");
    }
    sleep(2);
    system("ifconfig wlan0 down");
    sleep(2);
    system("killall wpa_supplicant");
    sleep(1);
    system("killall udhcpc");
    sleep(1);
    std::string wifi_conf = CFG_STR("paths", "wifi_conf", "/home/root/wifi.conf");
    std::string wpa_cmd = "wpa_supplicant -i wlan0 -B -c " + wifi_conf + " &";
    system(wpa_cmd.c_str());
    sleep(10);
    system("udhcpc -i wlan0 -t 8 -n");
    sleep(5);
    int fd_wifi_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_wifi_sock < 0)
    {
#ifdef DEBUG
        LOG_ERROR("dev_init", "%s", "wifi: fd_wifi_sock create failure");
#endif
    }
    else
    {
#ifdef DEBUG
        LOG_INFO("dev_init", "%s", "wifi: fd_wifi_sock creating");
        LOG_INFO("dev_init", "%s", "wifi: fd_wifi_sock create success");
#endif
        struct sockaddr_in wifi_ser_addr;
        memset(&wifi_ser_addr, 0x0, sizeof(struct sockaddr_in));
        wifi_ser_addr.sin_family = AF_INET;
        wifi_ser_addr.sin_port = htons(cfg_wifi_port());
        wifi_ser_addr.sin_addr.s_addr = inet_addr(cfg_wifi_ip().c_str());
        int wifi_fd_connect;
        wifi_fd_connect = connect_nonb(fd_wifi_sock, (struct sockaddr*)&wifi_ser_addr, sizeof(struct sockaddr_in), cfg_connect_timeout());
        if (wifi_fd_connect < 0)
        {
#ifdef DEBUG
            LOG_ERROR("dev_init", "wifi: connect: %s", strerror(errno));
#endif
            close(fd_wifi_sock);
            ctx.fds().wifi = -1;
        }
        else
        {
#ifdef DEBUG
            LOG_INFO("dev_init", "%s", "wifi: connect server success");
            LOG_INFO("dev_init", "Init success: wifi. Device descriptor = %d", fd_wifi_sock);
            ctx.fds().flag_wifi = 1;
#endif
            ctx.fds().wifi = fd_wifi_sock;
            ctx.commManager().addCommunicateNode(WIFI, ctx.fds().wifi);
            ctx.commManager().callbackRgist(WIFI, 0, wifi_reinit);
            ctx.fds().device_id.push_back(ctx.fds().wifi);
            ctx.identity().communicate_status[1] = '1';
            system("echo 1 > /sys/class/leds/yellow/brightness");
        }
    }

    ctx.fds().bt = bluetooth_open(ctx.identity().bt_mac);
    if (-1 == ctx.fds().bt)
    {
        ctx.identity().bt_mac = "";
        LOG_ERROR("dev_init", "%s", "-> No blue_tooth device");
    }
    else
    {
        ctx.commManager().addCommunicateNode(BT, ctx.fds().bt);
        ctx.commManager().callbackRgist(BT, 0, bt_reinit);
        ctx.fds().device_id.push_back(ctx.fds().bt);
#ifdef DEBUG
        LOG_INFO("dev_init", "Init success: bluetooth. Device descriptor = %d", ctx.fds().bt);
#endif
        ctx.identity().communicate_status[3] = '1';
        system("echo 1 > /sys/class/leds/green/brightness");
    }
    sleep(2);

    // 初始化网口1作为服务器端口，接收其他节点数据
    std::string eth0_ip = CFG_STR("network.lan", "zd_local_ip", "192.168.31.101");
    std::string ifconfig_eth0 = "ifconfig eth0 " + eth0_ip + " netmask 255.255.255.0";
    system(ifconfig_eth0.c_str());
    sleep(2);
    ctx.fds().lan_server = socket(AF_INET, SOCK_STREAM, 0);
    if (ctx.fds().lan_server < 0)
    {
#ifdef DEBUG
        LOG_ERROR("dev_init", "lan: socket create error: %s", strerror(errno));
#endif
    }
    else
    {
        struct sockaddr_in Llan_ser_addr;
        memset(&Llan_ser_addr, 0, sizeof(Llan_ser_addr));
        Llan_ser_addr.sin_family = AF_INET;
        Llan_ser_addr.sin_addr.s_addr = inet_addr(cfg_zd_lan_ip().c_str());
        Llan_ser_addr.sin_port = htons(cfg_zd_lan_port());
        if (bind(ctx.fds().lan_server, (struct sockaddr*)&Llan_ser_addr, sizeof(Llan_ser_addr)) < 0) {
            LOG_ERROR("dev_init", "bind fdL_lan failed: %s", strerror(errno));
            close(ctx.fds().lan_server);
            ctx.fds().lan_server = -1;
        } else {
            if (listen(ctx.fds().lan_server, 5) < 0) {
                LOG_ERROR("dev_init", "listen fdL_lan failed: %s", strerror(errno));
                close(ctx.fds().lan_server);
                ctx.fds().lan_server = -1;
            }
        }
#ifdef DEBUG
        LOG_INFO("dev_init", "Init success: LanLSS. Device descriptor = %d", ctx.fds().lan_server);
#endif
    }

    // 初始化网口2作为客户端，与ISR进行有线注册连接
    std::string eth1_ip = CFG_STR("network.lan", "local_ip", "192.168.2.235");
    std::string ifconfig_cmd = "ifconfig eth1 " + eth1_ip + " netmask 255.255.255.0";
    system(ifconfig_cmd.c_str());
    sleep(2);
    ctx.fds().lan = socket(AF_INET, SOCK_STREAM, 0);
    if (ctx.fds().lan < 0)
    {
#ifdef DEBUG
        LOG_ERROR("dev_init", "lan: socket create error: %s", strerror(errno));
#endif
    }
    else
    {
        struct sockaddr_in lan_ser_addr;
        memset(&lan_ser_addr, 0, sizeof(lan_ser_addr));
        lan_ser_addr.sin_family = AF_INET;
        lan_ser_addr.sin_addr.s_addr = inet_addr(cfg_lan_ip().c_str());
        lan_ser_addr.sin_port = htons(cfg_lan_port());
#ifdef DEBUG
        LOG_INFO("dev_init", "Init success: Lan. Device descriptor = %d", ctx.fds().lan);
#endif
        /* TODO: 此处使用阻塞connect，若对端不可达会长时间阻塞。建议后续改为 connect_nonb 非阻塞方式 */
        int lan_fd_connect = connect(ctx.fds().lan, (struct sockaddr*)&lan_ser_addr, sizeof(lan_ser_addr));
        if (lan_fd_connect < 0)
        {
#ifdef DEBUG
            LOG_ERROR("dev_init", "lan: connect: %s", strerror(errno));
#endif
            close(ctx.fds().lan);
            ctx.fds().lan = -1;
        }
        else
        {
#ifdef DEBUG
            LOG_INFO("dev_init", "%s", "lanISR: connect server success");
            LOG_INFO("dev_init", "Init success: Lan. Device descriptor = %d", ctx.fds().lan);
#endif
            ctx.commManager().addCommunicateNode(LAN, ctx.fds().lan);
            ctx.commManager().callbackRgist(LAN, 0, lan_reinit);
            ctx.fds().device_id.push_back(ctx.fds().lan);
            ctx.identity().communicate_status[2] = '1';
        }
    }

    if (ctx.fds().device_id.empty())
    {
        return -1;
    }
    return ctx.fds().device_id.size();
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
        ctx.identity().mac = std::string(buf, strlen(buf));
        /* 去除末尾换行符 */
        if (!ctx.identity().mac.empty() && ctx.identity().mac.back() == '\n') {
            ctx.identity().mac.pop_back();
        }
    }
    fclose(fp);
    return;
}
