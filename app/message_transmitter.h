/**
 * @file message_transmitter.h
 * @brief 消息传输模块接口
 * @details 提供消息发送线程，监听通信设备接收数据并转发到网关
 */
#pragma once
#ifndef _MESSAGE_TRANSMITTER_H_
#define _MESSAGE_TRANSMITTER_H_

/**
 * @brief 消息发送线程，监听通信设备接收数据并转发到网关
 * @param arg CSoftwareWdt软件看门狗对象指针
 * @return NULL
 */
void* send_mess(void* arg);

#endif
