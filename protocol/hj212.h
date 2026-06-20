/**
 * @file hj212.h
 * @brief HJ212 协议工具函数接口
 * @details 提供 HJ212 协议数据包封装、CRC16 校验、时间戳生成等工具函数
 */
#pragma once
#ifndef _HJ212_H_
#define _HJ212_H_

#include <string>
#include <stdint.h>
/**
 * @brief 将整数格式化为4位字符串，不足前补零
 * @param param 待格式化的整数
 * @return 4位字符串，如 "0012"
 */
std::string ensureLen_4(int param);

/**
 * @brief 封装完整的HJ212数据包（含CRC校验码补零）
 * @param crc       CRC16校验码
 * @param len_str   数据段长度（4位字符串）
 * @param data      数据段内容
 * @return 格式化后的HJ212数据包字符串（##+长度+数据+CRC）
 */
std::string ensure_crc4_packet(unsigned int crc, const std::string& len_str, const std::string& data);

/**
 * @brief CRC16循环冗余校验算法
 * @param puchMsg   待校验数据指针
 * @param usDataLen 数据长度
 * @return CRC16校验码
 */
unsigned int CRC16_Checkout(unsigned char* puchMsg, unsigned int usDataLen);

/**
 * @brief 获取SAP唯一标识MN
 * @return MN字符串
 */
std::string get_MN();

/**
 * @brief 获取当前时间戳字符串（精确到毫秒）
 * @return 格式为"yyyyMMddHHmmssSSS"的时间字符串
 */
std::string time_now_to_string();

/**
 * @brief 将int转换为2位格式字符串（如1->"01"，11->"11"）
 * @param source 输入整数
 * @param dest   输出缓冲区（至少3字节）
 */
void string_formater(int source, char* dest);

/**
 * @brief 将int转换为3位格式字符串（如1->"001"，11->"011"）
 * @param source 输入整数
 * @param dest   输出缓冲区（至少4字节）
 */
void string_formater_usec(int source, char* dest);

#endif
