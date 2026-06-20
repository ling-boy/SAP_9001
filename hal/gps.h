/**
 * @file gps.h
 * @brief GPS 数据采集模块接口
 * @details 通过串口读取 NMEA 语句，解析经纬度信息，提供线程安全的访问接口
 */
#pragma once
#ifndef _GPS_H_
#define _GPS_H_

#include <string>
#include <pthread.h>

/**
 * @brief GPS 数据获取线程入口函数
 * @param arg 未使用
 * @return NULL
 */
void* GET_GPS(void* arg);

/**
 * @brief 获取当前纬度（线程安全）
 * @return 纬度字符串，如 "N2932123"
 */
std::string gps_get_latitude();

/**
 * @brief 获取当前经度（线程安全）
 * @return 经度字符串，如 "E106361234"
 */
std::string gps_get_longitude();

/**
 * @brief 获取经纬度组合字符串（线程安全）
 * @return 格式 "纬度经度"，用于协议包
 */
std::string gps_get_location();

#endif
