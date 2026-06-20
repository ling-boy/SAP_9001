/**
 * @file device_init.cpp
 * @brief 设备初始化模块实现
 * @details 提供通信设备初始化、重初始化、MAC读取等功能
 */
#include "app/device_init.h"
#include "hal/lora.h"
#include "hal/bluetooth.h"
#include "infra/communica_manage.h"
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
using namespace std;

/* 外部全局变量（在 modbus_9001.cpp 中定义） */
extern int fd_lora, fd_wifi, fd_bt, fd_lan, fdL_lan;
extern vector<int> device_id;
extern string mac, bt_mac;
extern char communicate_status[];
extern int flag_wifi;
extern communicaManage* CM;
extern struct sockaddr_in wifi_ser_addr, lan_ser_addr, Llan_ser_addr;

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
        perror("fcntl F_GETFL");
        return -1;
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
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
        perror("select");
        return -1;
    }

    if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
        len = sizeof(error);
        if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
            return -1;
        }
    }
    else {
        fprintf(stderr, "select error: socket not set");
        return -1;
    }

done:
    if (fcntl(sockfd, F_SETFL, flags) == -1) {
        perror("fcntl");
    }

    if (error) {
        errno = error;
        return -1;
    }

    return 0;
}

int lora_reinit(int old_fd)
{
    close(old_fd);
    fd_lora = lora_open();
    if (fd_lora == -1)
    {
        cout << "lora重新初始化失败" << endl;
        return -1;
    }
    else
    {
        communicate_status[0] = '1';
        cout << "lora重新初始化成功" << endl;
        return fd_lora;
    }
}

int wifi_reinit(int old_fd)
{
    close(old_fd);
    if (system("ifconfig wlan0 down") != 0)
        printf("wifi_reinit: ifconfig wlan0 down failed\n");
    sleep(2);
    if (system("killall wpa_supplicant") != 0)
        printf("wifi_reinit: killall wpa_supplicant failed\n");
    sleep(1);
    if (system("killall udhcpc") != 0)
        printf("wifi_reinit: killall udhcpc failed\n");
    sleep(1);
    if (system("wpa_supplicant -i wlan0 -B -c /home/root/wifi.conf &") != 0)
        printf("wifi_reinit: wpa_supplicant start failed\n");
    sleep(10);
    if (system("udhcpc -i wlan0 -t 8 -n") != 0)
        printf("wifi_reinit: udhcpc failed\n");
    int fd_wifi_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_wifi_sock < 0)
    {
        printf("wifi: socket create failure");
        return -1;
    }
    else
    {
        printf("wifi: socket create success\n");
        memset(&wifi_ser_addr, 0x0, sizeof(struct sockaddr_in));
        wifi_ser_addr.sin_family = AF_INET;
        wifi_ser_addr.sin_port = htons(WIFI_SERVER_PORT);
        wifi_ser_addr.sin_addr.s_addr = inet_addr(WIFI_SERVER_IP);
        int wifi_fd_connect;
        wifi_fd_connect = connect_nonb(fd_wifi_sock, (struct sockaddr*)&wifi_ser_addr, sizeof(struct sockaddr_in), CONNECT_TIMEOUT_SEC);
        if (wifi_fd_connect < 0)
        {
            perror("wifi: connect");
            close(fd_wifi_sock);
            ::fd_wifi = -1;
            system("killall wpa_supplicant");
            system("killall udhcpc");
            return -1;
        }
        else
        {
            printf("wifi: connect server success\n");
            printf("Init success: wifi. Device descriptor = %d\n", fd_wifi_sock);
            ::fd_wifi = fd_wifi_sock;
            communicate_status[1] = '1';
            system("echo 1 > /sys/class/leds/yellow/brightness");
            return fd_wifi_sock;
        }
    }
}

int bt_reinit(int old_fd)
{
    close(old_fd);
    fd_bt = bluetooth_open(bt_mac);
    if (-1 == fd_bt)
    {
        bt_mac = "";
        printf("-> No blue_tooth device\n");
        return -1;
    }
    else
    {
        printf("Init success：bluetooth. Device descriptor = %d\n", fd_bt);
        communicate_status[3] = '1';
        system("echo 1 > /sys/class/leds/green/brightness");
        return fd_bt;
    }
}

int lan_reinit(int old_fd)
{
    close(old_fd);
    system("ifconfig eth1 192.168.2.235 netmask 255.255.255.0");
    sleep(2);
    fd_lan = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_lan < 0)
    {
        printf("lan: socket create error: %s\n", strerror(errno));
        return -1;
    }
    else
    {
        memset(&lan_ser_addr, 0, sizeof(lan_ser_addr));
        lan_ser_addr.sin_family = AF_INET;
        lan_ser_addr.sin_addr.s_addr = inet_addr(LAN_SERVER_IP);
        lan_ser_addr.sin_port = htons(LAN_SERVER_PORT);
        printf("Init success: Lan. Device descriptor = %d\n", fd_lan);
        /* TODO: 此处使用阻塞connect，若对端不可达会长时间阻塞。建议后续改为 connect_nonb 非阻塞方式 */
        int lan_fd_connect = connect(fd_lan, (struct sockaddr*)&lan_ser_addr, sizeof(lan_ser_addr));
        if (lan_fd_connect < 0)
        {
            perror("lan: connect");
            close(fd_lan);
            return -1;
        }
        else
        {
            printf("lanship: connect server success\n");
            printf("Init success: Lan. Device descriptor = %d\n", fd_lan);
            communicate_status[2] = '1';
            return fd_lan;
        }
    }
}

int dev_init()
{
    fd_lora = lora_open();
    if (-1 == fd_lora)
    {
#ifdef DEBUG
        printf("--> No lora device\n");
#endif
    }
    else
    {
        device_id.push_back(fd_lora);
        CM->addCommunicateNode(LORA, fd_lora);
        CM->callbackRgist(LORA, 0, lora_reinit);
        printf("Init success: lora. Device descriptor = %d\n", fd_lora);
        communicate_status[0] = '1';
        system("echo 1 > /sys/class/leds/red/brightness");
    }
    sleep(2);
    system("ifconfig wlan0 down");
    sleep(2);
    system("killall wpa_supplicant");
    sleep(1);
    system("killall udhcpc");
    sleep(1);
    system("wpa_supplicant -i wlan0 -B -c /home/root/wifi.conf &");
    sleep(10);
    system("udhcpc -i wlan0 -t 8 -n");
    sleep(5);
    int fd_wifi_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_wifi_sock < 0)
    {
#ifdef DEBUG
        printf("wifi: ======fd_wifi_sock create failure");
#endif
    }
    else
    {
#ifdef DEBUG
        printf("wifi: >>>>>>>>>>>>>>>>>>>>>>>>>>>");
        printf("wifi: fd_wifi_sock create success\n");
#endif
        memset(&wifi_ser_addr, 0x0, sizeof(struct sockaddr_in));
        wifi_ser_addr.sin_family = AF_INET;
        wifi_ser_addr.sin_port = htons(WIFI_SERVER_PORT);
        wifi_ser_addr.sin_addr.s_addr = inet_addr(WIFI_SERVER_IP);
        int wifi_fd_connect;
        wifi_fd_connect = connect_nonb(fd_wifi_sock, (struct sockaddr*)&wifi_ser_addr, sizeof(struct sockaddr_in), CONNECT_TIMEOUT_SEC);
        if (wifi_fd_connect < 0)
        {
#ifdef DEBUG
            perror("wifi: connect");
#endif
            close(fd_wifi_sock);
            fd_wifi = -1;
        }
        else
        {
#ifdef DEBUG
            printf("wifi: connect server success\n");
            printf("Init success: wifi. Device descriptor = %d\n", fd_wifi_sock);
            flag_wifi = 1;
#endif
            fd_wifi = fd_wifi_sock;
            CM->addCommunicateNode(WIFI, fd_wifi);
            CM->callbackRgist(WIFI, 0, wifi_reinit);
            device_id.push_back(fd_wifi);
            communicate_status[1] = '1';
            system("echo 1 > /sys/class/leds/yellow/brightness");
        }
    }

    fd_bt = bluetooth_open(bt_mac);
    if (-1 == fd_bt)
    {
        bt_mac = "";
        printf("-> No blue_tooth device\n");
    }
    else
    {
        CM->addCommunicateNode(BT, fd_bt);
        CM->callbackRgist(BT, 0, bt_reinit);
        device_id.push_back(fd_bt);
#ifdef DEBUG
        printf("Init success：bluetooth. Device descriptor = %d\n", fd_bt);
#endif
        communicate_status[3] = '1';
        system("echo 1 > /sys/class/leds/green/brightness");
    }
    sleep(2);

    // 初始化网口1作为服务器端口，接收其他节点数据
    system("ifconfig eth0 192.168.31.101 netmask 255.255.255.0");
    sleep(2);
    fdL_lan = socket(AF_INET, SOCK_STREAM, 0);
    if (fdL_lan < 0)
    {
#ifdef DEBUG
        printf("lan: socket create error: %s\n", strerror(errno));
#endif
    }
    else
    {
        memset(&Llan_ser_addr, 0, sizeof(Llan_ser_addr));
        Llan_ser_addr.sin_family = AF_INET;
        Llan_ser_addr.sin_addr.s_addr = inet_addr(ZD_LAN_SERVER_IP);
        Llan_ser_addr.sin_port = htons(ZD_LAN_SERVER_PORT);
        if (bind(fdL_lan, (struct sockaddr*)&Llan_ser_addr, sizeof(Llan_ser_addr)) < 0) {
            perror("bind fdL_lan failed");
            close(fdL_lan);
            fdL_lan = -1;
        } else {
            if (listen(fdL_lan, 5) < 0) {
                perror("listen fdL_lan failed");
                close(fdL_lan);
                fdL_lan = -1;
            }
        }
#ifdef DEBUG
        printf("Init success: LanLSS. Device descriptor = %d\n", fdL_lan);
#endif
    }

    // 初始化网口2作为客户端，与ISR进行有线注册连接
    system("ifconfig eth1 192.168.2.235 netmask 255.255.255.0");
    sleep(2);
    fd_lan = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_lan < 0)
    {
#ifdef DEBUG
        printf("lan: socket create error: %s\n", strerror(errno));
#endif
    }
    else
    {
        memset(&lan_ser_addr, 0, sizeof(lan_ser_addr));
        lan_ser_addr.sin_family = AF_INET;
        lan_ser_addr.sin_addr.s_addr = inet_addr(LAN_SERVER_IP);
        lan_ser_addr.sin_port = htons(LAN_SERVER_PORT);
#ifdef DEBUG
        printf("Init success: Lan. Device descriptor = %d\n", fd_lan);
#endif
        /* TODO: 此处使用阻塞connect，若对端不可达会长时间阻塞。建议后续改为 connect_nonb 非阻塞方式 */
        int lan_fd_connect = connect(fd_lan, (struct sockaddr*)&lan_ser_addr, sizeof(lan_ser_addr));
        if (lan_fd_connect < 0)
        {
#ifdef DEBUG
            perror("lan: connect");
#endif
            close(fd_lan);
            fd_lan = -1;
        }
        else
        {
#ifdef DEBUG
            printf("lanISR: connect server success\n");
            printf("Init success: Lan. Device descriptor = %d\n", fd_lan);
#endif
            CM->addCommunicateNode(LAN, fd_lan);
            CM->callbackRgist(LAN, 0, lan_reinit);
            device_id.push_back(fd_lan);
            communicate_status[2] = '1';
        }
    }

    if (device_id.empty())
    {
        return -1;
    }
    return device_id.size();
}

void get_mac()
{
    FILE* fp = fopen("./mac.txt", "r");
    if (fp == NULL)
    {
        printf("get mac failed\n");
        return;
    }
    char buf[256] = {0};
    if (fgets(buf, sizeof(buf), fp) != NULL) {
        mac = string(buf, strlen(buf));
        /* 去除末尾换行符 */
        if (!mac.empty() && mac.back() == '\n') {
            mac.pop_back();
        }
    }
    fclose(fp);
    return;
}
