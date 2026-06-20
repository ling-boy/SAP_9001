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
#include <iostream>
#include <vector>
#include <sys/time.h>
#include "infra/software_wdt.h"
#include "infra/communica_manage.h"
using namespace std;

/** 最大协议包字节数 */
#define RX_SIZE                 58
/** 注册请求 */
#define REQ_REGISTER            "01"
/** 注册确认 */
#define REQ_REG_CONFIRM         "02"
/** 数据请求 */
#define REQ_DATA                "03"
/** 数据上传确认 */
#define REQ_TXD_CONFIRM         "04"
/** 传感器参数变更执行确认 */
#define REQ_PARAM_CONFIRM       "05"
/** 传感器参数变更命令下发 */
#define REQ_PARAM_SEND          "06"
/** 接入设备ID变更执行确认 */
#define REQ_ID_CONFIRM          "07"
/** 接入设备ID变更命令下发 */
#define REQ_ID_SEND             "08"
/** 监测终端重置执行确认 */
#define REQ_MONI_RET_CONFIRM    "09"
/** 监测终端重置命令下发 */
#define REQ_MONI_RET            "10"
/** 接入设备重启执行确认 */
#define REQ_ACC_RET_CONFIRM     "11"
/** 接入设备重启命令下发 */
#define REQ_ACC_RET             "12"
/** 更新通信秘钥执行确认 */
#define REQ_KEY_CONFIRM         "13"
/** 更新通信秘钥命令下发 */
#define REQ_KEY_SEND            "14"
/** 请求通信秘钥 */
#define REQ_ASK_KEY             "15"
/** 下发通信秘钥 */
#define REQ_SEND_KEY            "16"
/** 网关参数下发 */
#define REQ_SEND_INFO           "17"
/** 接入设备退网 */
#define REQ_ACC_CLOSE           "18"
/** 网关设备退网 */
#define REQ_GATEWAY_CLOSE       "19"
/** 接入设备心跳包 */
#define REQ_ACC_HEART           "EF"
/** 网关设备心跳包 */
#define REQ_GATE_HEART          "FF"
/** 时间戳下发 */
#define TIME_SEND               "20"

extern string  id, net_id, mac, cpu_occupy, Isr_mac, current_time;
extern char communicate_status[];
extern int monitor_time;
extern communicaManage* CM;

/**
 * @brief 设备注册线程参数结构体
 */
struct deviceRegist
{
	CSoftwareWdt *g_CsoftwareWdt;           /**< 软件看门狗对象 */
	vector<vector<int>> *vec;                /**< 初始化成功的通信设备列表 */
	deviceRegist(CSoftwareWdt *g_CsoftwareWdt,vector<vector<int>> *vec) :g_CsoftwareWdt(g_CsoftwareWdt), vec(vec) {};
};

/**
 * @brief 设备注册线程函数（SAP注册）
 * @param arg deviceRegist结构体指针
 * @return NULL
 */
void* device_regist(void *arg);
#endif
