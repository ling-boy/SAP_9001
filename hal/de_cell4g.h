/**
 * @file de_cell4g.h
 * @brief 4G蜂窝网络模块接口
 * @details 提供4G模块初始化、PPP拨号、IP检测和TCP连接测试功能
 */
#ifndef __DE_CELL4G_H_
#define __DE_CELL4G_H_

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>

/**
 * @brief 检测4G网络是否已成功获取到IP地址
 * @param net_name 网络接口名称，例如 "ppp0"
 * @return 成功返回0，失败返回-1
 */
int cell4g_detect(char* net_name);

/**
 * @brief 通过TCP连接测试指定的服务器IP和端口是否可达
 * @param remote_ip   远程服务器IP地址
 * @param remote_port 远程服务器端口号
 * @return 成功返回套接字fd，失败返回-1
 */
int cell4g_ip_port_check(const char* remote_ip, int remote_port);

/**
 * @brief 打开4G模块并连接到指定服务器
 * @return 成功返回套接字fd，失败返回-1
 */
int cell4g_open(void);

/**
 * @brief 初始化4G模块，通过PPP拨号建立蜂窝网络连接
 * @param net_name 网络接口名称，例如 "ppp0"
 * @return 成功返回0，失败返回-1
 */
int hard4g_init(char* net_name);

#endif
