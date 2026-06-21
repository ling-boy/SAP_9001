/**
 * @file usart.h
 * @brief 串口监听模块接口
 * @details 提供RS485和RS232串口数据监听功能，使用select多路复用
 *          注意：RS485_Monitor 功能尚未集成，保留接口定义供后续使用
 */
#pragma once
#ifndef _USART_H_
#define _USART_H_
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>
#include <string>
#include "hal/usbctl.h"
#include <queue>
#include "infra/software_wdt.h"

/** RS485端口A标识 */
#define RS485_A                 "100"
/** RS485端口B标识 */
#define RS485_B                 "010"
/** RS485端口C标识 */
#define RS485_C                 "001"
/** RS232端口标识 */
#define RS232                   "101"
/** RJ45网口标识 */
#define RJ45                    "111"

/**
 * @brief 监听RS485和RS232串口数据，收到数据后封装为06包推送到队列
 * @param arg monitor485结构体指针
 * @return NULL
 * @note 此功能尚未集成到主程序，保留接口定义供后续使用
 */
void* RS485_Monitor(void *arg);

/**
 * @brief RS485监听线程参数结构体
 * @note 此功能尚未集成到主程序，保留结构体定义供后续使用
 */
struct monitor485
{
	std::queue<std::string> *que;             /**< 数据包队列 */
	CSoftwareWdt *g_CsoftwareWdt;  /**< 软件看门狗对象 */
	monitor485(std::queue<std::string> *que, CSoftwareWdt *g_CsoftwareWdt) : que(que), g_CsoftwareWdt(g_CsoftwareWdt) {};
};
#endif
