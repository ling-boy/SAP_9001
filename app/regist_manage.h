/**
 * @file registManage.h
 * @brief 设备注册管理模块接口
 * @details 通过select监听通信设备，发送注册请求，等待ISR下发17协议参数和20协议时间戳
 */
#pragma once
#ifndef _REGISTMANAGE_H_
#define _REGISTMANAGE_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <string>
#include <vector>
#include <sys/time.h>
#include "infra/software_wdt.h"
#include "infra/communica_manage.h"
#include "protocol/constants.h"

/** 最大协议包字节数 */
#define RX_SIZE                 58

/* 所有全局状态已迁移至 DeviceContext 单例，通过 sap::DeviceContext::instance() 访问 */

/**
 * @brief 设备注册线程参数结构体
 */
struct deviceRegist
{
	CSoftwareWdt *g_CsoftwareWdt;           /**< 软件看门狗对象 */
	std::vector<std::vector<int>> *vec;      /**< 初始化成功的通信设备列表 */
	deviceRegist(CSoftwareWdt *g_CsoftwareWdt, std::vector<std::vector<int>> *vec) :g_CsoftwareWdt(g_CsoftwareWdt), vec(vec) {};
};

/**
 * @brief 设备注册线程函数（SAP注册）
 * @param arg deviceRegist结构体指针
 * @return NULL
 */
void* device_regist(void *arg);
#endif
