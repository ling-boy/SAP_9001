/**
 * @file lora.h
 * @brief LoRa无线通信模块接口
 * @details 提供LoRa和RS485串口设备的打开和配置功能
 */
#pragma once
#ifndef _LORA_H_
#define _LORA_H_

/**
 * @brief 打开LoRa串口设备并配置通信参数
 * @return 成功返回串口fd，失败返回-1
 */
int lora_open();

#endif
