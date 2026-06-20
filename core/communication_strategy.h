/**
 * @file communication_strategy.h
 * @brief 通信策略接口（策略模式）
 * @details 定义所有通信方式的统一接口，支持 LoRa/WiFi/BT/LAN/4G
 *
 * 使用示例：
 *   class LoraStrategy : public ICommunicationStrategy {
 *   public:
 *       int initialize() override { return lora_open(); }
 *       int reinitialize(int old_fd) override { return lora_reinit(old_fd); }
 *       std::string typeName() const override { return "LoRa"; }
 *       int deviceId() const override { return 1; }
 *   };
 *
 * @version 1.0
 */
#pragma once

#include <string>
#include <functional>

namespace sap {

/**
 * @brief 通信策略接口
 * @details 定义所有通信方式的统一接口
 */
class ICommunicationStrategy {
public:
    virtual ~ICommunicationStrategy() = default;

    /**
     * @brief 初始化通信设备
     * @return 文件描述符，-1 表示失败
     */
    virtual int initialize() = 0;

    /**
     * @brief 重新初始化（连接断开后）
     * @param old_fd 旧的文件描述符
     * @return 新的文件描述符，-1 表示失败
     */
    virtual int reinitialize(int old_fd) = 0;

    /**
     * @brief 获取设备类型标识
     */
    virtual std::string typeName() const = 0;

    /**
     * @brief 获取设备 ID（用于 communicate_status 数组索引）
     */
    virtual int deviceId() const = 0;

    /**
     * @brief 检查是否已连接
     */
    virtual bool isConnected() const = 0;

    /**
     * @brief 获取当前文件描述符
     */
    virtual int fileDescriptor() const = 0;

    /**
     * @brief 设置文件描述符
     */
    virtual void setFileDescriptor(int fd) = 0;
};

/**
 * @brief 通信策略基类（提供通用实现）
 */
class CommunicationStrategyBase : public ICommunicationStrategy {
public:
    bool isConnected() const override { return fd_ >= 0; }
    int fileDescriptor() const override { return fd_; }
    void setFileDescriptor(int fd) override { fd_ = fd; }

protected:
    int fd_ = -1;
};

} // namespace sap
