/**
 * @file get.h
 * @brief 系统资源监控模块接口
 * @details 提供CPU使用率和内存使用率的采集与计算功能
 */
#ifndef _GET_H_
#define _GET_H_

#pragma once
#include <iostream>
#include <string>
#include <sstream>
/**
 * @brief 内存信息结构体，对应/proc/meminfo中的关键字段
 */
typedef struct MEMPACKED
{
    char name1[20];
    unsigned long MemTotal;     /**< 总内存 (kB) */
    char name2[20];
    unsigned long MemFree;      /**< 空闲内存 (kB) */
    char name3[20];
    unsigned long Buffers;      /**< 缓冲区 (kB) */
    char name4[20];
    unsigned long Cached;       /**< 缓存 (kB) */
    char name5[20];
    unsigned long SwapCached;   /**< 交换缓存 (kB) */
}MEM_OCCUPY;

/**
 * @brief CPU信息结构体，对应/proc/stat中的CPU时间字段
 */
typedef struct CPUPACKED
{
    char name[20];
    unsigned int user;      /**< 用户态时间 */
    unsigned int nice;      /**< 低优先级用户态时间 */
    unsigned int system;    /**< 内核态时间 */
    unsigned int idle;      /**< 空闲时间 */
    unsigned int lowait;    /**< I/O等待时间 */
    unsigned int irq;       /**< 硬中断时间 */
    unsigned int softirq;   /**< 软中断时间 */
}CPU_OCCUPY;

/**
 * @brief 从/proc/meminfo读取系统内存使用信息
 * @param mem 输出参数，存储内存信息
 */
void get_memoccupy(MEM_OCCUPY *mem);

/**
 * @brief 从/proc/stat读取CPU使用信息
 * @param cpust 输出参数，存储CPU信息
 * @return 成功返回0
 */
int get_cpuoccupy(CPU_OCCUPY *cpust);

/**
 * @brief 计算内存使用率百分比
 * @param m 内存信息结构体指针
 * @return 内存使用率百分比字符串
 */
std::string cal_memoccupy(MEM_OCCUPY *m);

/**
 * @brief 通过两次CPU采样计算CPU使用率百分比
 * @param o 第一次采样的CPU信息
 * @param n 第二次采样的CPU信息
 * @return CPU使用率百分比字符串
 */
std::string cal_cpuoccupy(CPU_OCCUPY *o, CPU_OCCUPY *n);

/**
 * @brief 获取CPU和内存使用率的组合字符串
 * @return 格式为4位的CPU+内存使用率字符串
 */
std::string get_cpuOccupy();

#endif
