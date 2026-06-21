/**
 * @file protocol_process.h
 * @brief 协议处理模块接口
 * @details 提供HJ212协议和私有协议的数据包封装、解析和持久化功能
 */
#pragma once
#ifndef _PROTOCOL_PROCESS_H_
#define _PROTOCOL_PROCESS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include <iomanip>
#include <memory.h>
#include <fcntl.h>
#include <errno.h>
#include "infra/communica_manage.h"
#include "infra/get.h"
#include "infra/message_queue.h"
#include <queue>
/**
 * @brief 将0-15的整数转换为对应的十六进制字符
 * @param num 整数（0~15）
 * @return 十六进制字符字符串
 */
std::string switch10_16(int num);

/**
 * @brief 递归将十进制数转换为十六进制字符串
 * @param num 十进制数
 * @param str 输出的十六进制字符串
 * @return 始终返回0
 */
int change(long long int num, std::string& str);

/**
 * @brief 封装私有协议06包
 * @param strT     数据内容
 * @param portInfo 端口信息标识
 * @return 封装后的06协议数据包
 */
std::string packet06(std::string strT, std::string portInfo);

/**
 * @brief 处理20协议（时间戳下发）
 * @param strPacket 20协议数据包
 */
void packet20(std::string& strPacket);

/**
 * @brief 将队列中的所有数据包持久化到文件（用于信号处理等紧急场景）
 * @param buffer 数据包队列引用
 * @return 成功写入的数据包数量
 */
int drainAndPersist(MessageQueue<std::string>& buffer);

/**
 * @brief 无锁版本的 drainAndPersist（仅用于信号处理器，避免死锁）
 * @warning 非线程安全，仅在 async-signal-safe 场景下使用
 */
int drainAndPersistUnsafe(MessageQueue<std::string>& buffer);

/**
 * @brief 从本地文件读取未发送的数据包，并将06包转为21重发包
 * @param recvQueue 输出队列，存储读取的数据包
 * @return 成功返回0，失败返回-1
 */
int readFromFile(std::queue<std::string>& recvQueue);

/**
 * @brief 构建心跳包（EF协议）
 * @details 心跳包包含设备状态信息：CPU使用率、内存空闲率、通信状态、队列深度
 *          格式：$EF + 通信类型 + 设备ID + 网络ID + 00 + 长度 + CPU + 内存 + 通信状态 + 队列深度 + @
 * @return 心跳包字符串
 */
std::string buildHeartbeat();

#endif
