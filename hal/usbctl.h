/**
 * @file usbctl.h
 * @brief 通用USB串口控制模块接口
 * @details 提供串口打开、配置、读写等基础操作接口声明
 */
#ifndef _USBCTL_H_
#define _USBCTL_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @brief 打开指定编号的USB串口设备（阻塞模式）
 * @param comport 串口编号（0=LoRa, 1=WiFi, 2=蓝牙, 3=4G, 4~6=RS485, 7=RS232）
 * @return 成功返回文件描述符fd(>0)，失败返回-1
 */
int open_port(int comport);

/**
 * @brief 配置串口参数（通用版本）
 * @param fd      文件描述符
 * @param nSpeed  波特率：2400/4800/9600/115200/460800，默认9600
 * @param nBits   数据位：7 或 8
 * @param nEvent  校验位：'o'奇校验 / 'e'偶校验 / 'n'无校验
 * @param nStop   停止位：1 或 2
 * @return 成功返回0，失败返回-1
 */
int set_opt1(int fd, int nSpeed, int nBits, uint8_t nEvent, int nStop);

/**
 * @brief 配置串口参数（支持硬件流控，主要用于RS485端口）
 * @param fd      文件描述符
 * @param nSpeed  波特率：2400/4800/9600/115200/460800，默认9600
 * @param nBits   数据位：7 或 8
 * @param nEvent  校验位：'o'奇校验 / 'e'偶校验 / 'n'无校验
 * @param nStop   停止位：1 或 2
 * @param flag    硬件流控：1开启CRTSCTS / 0关闭
 * @return 成功返回0，失败返回-1
 */
int set_opt(int fd, int nSpeed, int nBits, uint8_t nEvent, int nStop, int flag);

/**
 * @brief 向串口写入数据
 * @param fd  文件描述符
 * @param buf 待发送数据的缓冲区首地址
 * @param len 待发送数据的字节长度
 * @return 成功返回已发送字节数，失败返回-1
 */
int write_port(int fd, const void *buf, size_t len);

/**
 * @brief 从串口读取数据
 * @param fd  文件描述符
 * @param buf 接收数据的缓冲区首地址
 * @param len 期望读取的最大字节数，必须小于缓冲区大小
 * @return 成功返回已读取字节数，失败返回-1
 */
int read_port(int fd, void *buf, size_t len);

#endif
