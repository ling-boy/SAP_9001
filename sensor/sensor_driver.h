/**
 * @file sensor_driver.h
 * @brief 传感器驱动基类接口
 * @details 提供统一的 Modbus RTU 传感器轮询框架，消除船载/大气传感器的重复代码
 */
#pragma once
#ifndef _SENSOR_DRIVER_H_
#define _SENSOR_DRIVER_H_

#include <string>
#include <vector>
#include <stdint.h>
#include <modbus/modbus.h>
#include "infra/software_wdt.h"

/** @brief HJ212 协议传感器值结构体 */
struct SensorValue {
    std::string code;     ///< HJ212 编码，如 "a21026"
    double value;    ///< 传感器值
};

/** @brief Modbus 寄存器转换方式 */
enum class ConversionType {
    FLOAT32,           ///< 4字节浮点数（IEEE 754），保持寄存器
    FLOAT32_SWAP,      ///< 4字节浮点数（字节序交换），保持寄存器
    UINT16_SCALE,      ///< 16位无符号整数 * 缩放因子，保持寄存器
    UINT16_RAW,        ///< 16位无符号整数（原始值），保持寄存器
    INPUT_REG_FLOAT32, ///< 4字节浮点数，输入寄存器（功能码 04）
};

/** @brief 单个传感器的 Modbus 配置 */
struct SensorConfig {
    int slave_address;         ///< Modbus 从站地址
    int register_address;      ///< 起始寄存器地址
    int register_count;        ///< 读取寄存器数量
    ConversionType conversion; ///< 数据转换方式
    double scale_factor;       ///< 缩放因子（仅 UINT16_SCALE 使用）
    std::string hj212_code;    ///< HJ212 协议编码
    std::string name;          ///< 传感器名称（用于日志）
    int sleep_seconds;         ///< 读取后等待时间（秒）
    const char* rtd_format;    ///< HJ212 Rtd 格式字符串（如 "%010.3lf" 或 "%08.2lf"）
};

/**
 * @brief 传感器驱动基类
 * @details 封装 Modbus RTU 轮询 + HJ212 打包的通用流程
 *          子类只需提供传感器配置列表和 HJ212 ST 字段
 */
class SensorDriver {
public:
    /**
     * @brief 构造函数
     * @param wdt       软件看门狗指针
     * @param wdt_id    看门狗 ID（由子类分配）
     * @param port_info RS485 端口信息标识（如 "100" 表示 RS485_A）
     * @param device    Modbus 设备路径（如 "/dev/ttymxc2"）
     * @param baud      波特率
     */
    SensorDriver(CSoftwareWdt* wdt, int wdt_id,
                 const std::string& port_info, const std::string& device, int baud = 9600);
    virtual ~SensorDriver();

    /**
     * @brief 线程入口函数（主循环）
     * @details 按配置列表轮询所有传感器，打包 HJ212 数据并推送到队列
     */
    void run();

protected:
    /**
     * @brief 获取传感器配置列表（由子类实现）
     * @return 传感器配置向量
     */
    virtual std::vector<SensorConfig> getSensorConfigs() = 0;

    /**
     * @brief 获取 HJ212 ST 字段（由子类实现）
     * @return ST 字符串，如 "26"（水质）或 "31"（大气）
     */
    virtual std::string getHJ212ST() = 0;

    /**
     * @brief 从 Modbus 寄存器读取传感器值
     * @param ctx    Modbus 上下文
     * @param config 传感器配置
     * @param value  输出值
     * @return 成功返回 true
     */
    bool readSensorValue(modbus_t* ctx, const SensorConfig& config, double& value);

    CSoftwareWdt* wdt_;
    int wdt_id_;
    std::string port_info_;
    std::string device_;
    int baud_;
};

/**
 * @brief 获取船载传感器数据线程入口
 * @param arg CSoftwareWdt 指针
 * @return NULL
 */
void* get_ship_data(void* arg);

/**
 * @brief 获取大气传感器数据线程入口
 * @param arg CSoftwareWdt 指针
 * @return NULL
 */
void* get_gassensor_data(void* arg);

#endif
