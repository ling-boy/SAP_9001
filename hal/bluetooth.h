/**
 * @file bluetooth.h
 * @brief 蓝牙通信模块接口
 * @details 提供蓝牙模块AT指令初始化、设备扫描和消息解析功能
 */
#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_
#include <string>

/**
 * @brief 打开蓝牙模块，扫描附近设备并查找目标设备
 * @param device_mac 输出参数，存储找到的设备MAC地址
 * @return 成功返回蓝牙串口fd，失败返回-1
 */
int bluetooth_open(std::string &device_mac);

/**
 * @brief 解析蓝牙扫描返回的消息，判断是否找到目标设备
 * @param meg 待解析的消息字符串
 * @return 1=未找到设备, 2=找到设备(MAC已提取), 3=扫描结束
 */
int blue_proc_message(std::string &meg);
#endif
