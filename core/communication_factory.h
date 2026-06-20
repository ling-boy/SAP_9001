/**
 * @file communication_factory.h
 * @brief 通信设备工厂（工厂模式）
 * @details 根据配置创建所有通信策略实例
 *
 * 使用示例：
 *   auto strategies = CommunicationFactory::createAll();
 *   for (auto& s : strategies) {
 *       int fd = s->initialize();
 *       if (fd >= 0) { ... }
 *   }
 *
 * @version 1.0
 */
#pragma once

#include <memory>
#include <vector>
#include <string>
#include "core/communication_strategy.h"

namespace sap {

/**
 * @brief 通信设备工厂
 */
class CommunicationFactory {
public:
    /**
     * @brief 创建所有启用的通信策略
     * @return 策略实例列表
     */
    static std::vector<std::unique_ptr<ICommunicationStrategy>> createAll();

    /**
     * @brief 创建指定类型的通信策略
     * @param type 类型名称（"lora", "wifi", "bt", "lan", "4g"）
     * @return 策略实例，未知类型返回 nullptr
     */
    static std::unique_ptr<ICommunicationStrategy> create(const std::string& type);
};

} // namespace sap
