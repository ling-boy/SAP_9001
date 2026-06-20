/**
 * @file device_init.h
 * @brief 设备初始化模块接口
 * @details 提供通信设备初始化、重初始化、MAC读取等功能
 */
#pragma once
#ifndef _DEVICE_INIT_H_
#define _DEVICE_INIT_H_

#include <sys/socket.h>
#include <string>
#include <vector>

/** @brief 网络配置常量 */
#define WIFI_SERVER_IP   "192.168.3.1"
#define WIFI_SERVER_PORT 1234
#define LAN_SERVER_IP    "192.168.2.1"
#define LAN_SERVER_PORT  2234
#define ZD_LAN_SERVER_IP   "192.168.31.101"
#define ZD_LAN_SERVER_PORT 3234
#define CONNECT_TIMEOUT_SEC  3

/**
 * @brief 程序退出处理函数，执行硬件资源清理
 * @param signum 信号编号
 */
void stop(int signum);

/**
 * @brief 非阻塞connect连接，带超时
 * @param sockfd  套接字描述符
 * @param saptr   远端地址
 * @param salen   远端地址长度
 * @param nsec    超时时间（秒）
 * @return 0 成功，-1 失败
 */
int connect_nonb(int sockfd, const struct sockaddr* saptr, socklen_t salen, int nsec);

/**
 * @brief 重新初始化LoRa通信设备
 * @param fd_lora LoRa文件描述符
 * @return 新的文件描述符，-1失败
 */
int lora_reinit(int old_fd);

/**
 * @brief 重新初始化WiFi通信设备
 * @param fd_wifi WiFi文件描述符
 * @return 新的文件描述符，-1失败
 */
int wifi_reinit(int old_fd);

/**
 * @brief 重新初始化蓝牙通信设备
 * @param fd_bt 蓝牙文件描述符
 * @return 新的文件描述符，-1失败
 */
int bt_reinit(int old_fd);

/**
 * @brief 重新初始化LAN有线通信设备
 * @param fd_lan LAN文件描述符
 * @return 新的文件描述符，-1失败
 */
int lan_reinit(int old_fd);

/**
 * @brief 初始化所有通信设备（LoRa、蓝牙、WiFi、LAN）
 * @return 成功初始化的设备数量，-1表示全部失败
 */
int dev_init();

/**
 * @brief 从文件中读取设备唯一MAC地址
 */
void get_mac();

#endif
