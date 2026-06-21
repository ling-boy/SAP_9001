/**
 * @file esp8266.h
 * @brief ESP8266 WiFi模块接口
 * @details 预留接口，供后续扩展。
 *          当前系统主要通过 WiFi STA 模式（wpa_supplicant）连接，
 *          ESP8266 AP 热点模式作为备选通信方案保留。
 *          提供ESP8266模块的AT指令检测、重启和AP热点配置功能
 */
#ifndef _ESP8266_H_
#define _ESP8266_H_

/**
 * @brief 打开ESP8266 WiFi模块并验证设备是否正常响应
 * @return 成功返回串口fd，失败返回-1
 */
int esp8266_open();

/**
 * @brief 配置ESP8266为AP热点模式并开启TCP服务器
 * @param fd ESP8266串口文件描述符
 * @return 成功返回0，失败返回-1
 */
int esp8266_config(int fd);
#endif
