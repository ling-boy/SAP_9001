/**
 * @file softwarewdt.h
 * @brief 软件看门狗模块接口
 * @details 提供线程级软件看门狗，支持多线程注册、喂狗、超时监控和自动重启
 */
#pragma once
#ifndef _SOFTWAREWDT_H_
#define _SOFTWAREWDT_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <syslog.h>
#include <iostream>
#include "protocol/protocol_process.h"
#include <pthread.h>
#include <signal.h>
#include <mutex>
/** 最大支持的看门狗注册数量 */
#define REQUEST_WDT_MAX_NUM	20

/** 全局线程ID声明，用于看门狗超时时取消对应线程 */
extern pthread_t tid_getSensor, tid_getLan, tid_transMessage, tid_gps, tid_ship_data, tid_softwd, tid_monitor485, tid_deviceRegist;

/**
 * @brief 软件看门狗类
 * @details 通过计数器机制监控各线程存活状态，超时则触发线程重启
 */
class CSoftwareWdt
{
private:
	bool m_fReqWdtFlg[REQUEST_WDT_MAX_NUM];    /**< 注册标志：true=已申请ID */
	int m_wWdtTimeout[REQUEST_WDT_MAX_NUM];     /**< 超时阈值（秒） */
	int m_wWdtCount[REQUEST_WDT_MAX_NUM];       /**< 当前计数器 */
	std::string m_wwdtName[REQUEST_WDT_MAX_NUM]; /**< 被监控线程名称 */
	/** @brief 可重入互斥锁，保护看门狗操作的线程安全 */
	std::recursive_mutex mtx_;

public:
	CSoftwareWdt();
	~CSoftwareWdt();

	/**
	 * @brief 申请软件看门狗ID
	 * @param cThreadName 线程名称
	 * @param wTimeout    超时时间（秒）
	 * @return 成功返回看门狗ID(0~19)，失败返回-1
	 */
	int RequestSoftwareWdtID(const char *cThreadName, int wTimeout);

	/**
	 * @brief 释放指定线程的看门狗ID
	 * @param cThreadName 线程名称
	 * @param wWdtId      看门狗ID
	 * @return 成功返回true，失败返回false
	 */
	bool ReleaseSoftwareWdtID(const char *cThreadName, int wWdtId);

	/**
	 * @brief 喂狗操作，重置指定看门狗的计数器
	 * @param wWdtId 看门狗ID
	 * @return 成功返回true
	 */
	bool KeepSoftwareWdtAlive(int wWdtId);

	/**
	 * @brief 监控所有注册线程的运行状态，超时则触发重启
	 * @return 正常返回0，有线程超时返回-1
	 */
	int MontiorWdtRunState();
};

/**
 * @brief 看门狗线程入口函数
 * @param arg CSoftwareWdt对象指针
 * @return NULL
 */
void* softwarewd(void* arg);
#endif
