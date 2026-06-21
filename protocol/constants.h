/**
 * @file constants.h
 * @brief 协议常量定义
 * @details 统一定义所有协议命令号，消除重复宏定义
 */
#pragma once

/** @brief 协议命令号定义 */
#define REQ_REGISTER            "01"    ///< 注册请求
#define REQ_REG_CONFIRM         "02"    ///< 注册确认
#define REQ_DATA                 "03"   ///< 数据请求
#define REQ_TXD_CONFIRM         "04"    ///< 数据上传确认
#define REQ_PARAM_CONFIRM       "05"    ///< 参数确认
#define REQ_PARAM_SEND          "06"    ///< 参数发送（传感器数据包）
#define REQ_ID_CONFIRM          "07"    ///< ID确认
#define REQ_ID_SEND             "08"    ///< ID发送
#define REQ_MONI_RET_CONFIRM    "09"    ///< 监测结果确认
#define REQ_MONI_RET            "10"    ///< 监测结果
#define REQ_ACC_RET_CONFIRM     "11"    ///< 累积数据确认
#define REQ_ACC_RET             "12"    ///< 累积数据
#define REQ_KEY_CONFIRM         "13"    ///< 密钥确认
#define REQ_KEY_SEND            "14"    ///< 密钥发送
#define REQ_ASK_KEY             "15"    ///< 请求密钥
#define REQ_SEND_KEY            "16"    ///< 发送密钥
#define REQ_SEND_INFO           "17"    ///< 网关参数下发
#define REQ_ACC_CLOSE           "18"    ///< 累积数据关闭
#define REQ_GATEWAY_CLOSE       "19"    ///< 网关关闭
#define TIME_SEND               "20"    ///< 时间戳下发
#define REQ_ACC_HEART           "EF"    ///< 接入设备心跳
#define REQ_GATE_HEART          "FF"    ///< 网关心跳

/** @brief 接收缓冲区大小（协议包最大长度） */
#define RX_SIZE                 58
