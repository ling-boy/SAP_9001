/**
 * @file device_init.h
 * @brief 设备初始化模块接口
 * @details 提供通信设备初始化、重初始化、MAC读取等功能
 *          网络配置已移至 config/device.conf，通过 ConfigManager 读取
 */
#pragma once
#ifndef _DEVICE_INIT_H_
#define _DEVICE_INIT_H_

#include <sys/socket.h>
#include <string>
#include <vector>
#include "infra/config.h"

/** @brief 网络配置常量（从配置文件读取，带默认值兜底） */
inline std::string cfg_wifi_ip()   { return CFG_STR("network.wifi", "server_ip", "192.168.3.1"); }
inline int          cfg_wifi_port(){ return CFG_INT("network.wifi", "server_port", 1234); }
inline std::string cfg_lan_ip()    { return CFG_STR("network.lan", "server_ip", "192.168.2.1"); }
inline int          cfg_lan_port() { return CFG_INT("network.lan", "server_port", 2234); }
inline std::string cfg_zd_lan_ip() { return CFG_STR("network.lan", "zd_server_ip", "192.168.31.101"); }
inline int          cfg_zd_lan_port(){ return CFG_INT("network.lan", "zd_server_port", 3234); }
inline int          cfg_connect_timeout(){ return CFG_INT("timeouts", "connect_sec", 3); }

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
