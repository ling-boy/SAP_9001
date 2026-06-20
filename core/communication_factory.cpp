/**
 * @file communication_factory.cpp
 * @brief 通信设备工厂实现
 */

#include "core/communication_factory.h"
#include "infra/logger.h"
#include "infra/config.h"
#include "hal/lora_strategy.h"
#include "hal/wifi_strategy.h"
#include "hal/bt_strategy.h"
#include "hal/lan_strategy.h"
#include "hal/cell4g_strategy.h"

namespace sap {

std::vector<std::unique_ptr<ICommunicationStrategy>> CommunicationFactory::createAll() {
    std::vector<std::unique_ptr<ICommunicationStrategy>> strategies;

    // 根据配置决定启用哪些通信方式
    // 默认全部启用，可通过配置文件禁用

    if (CFG_INT("communication", "enable_lora", 1)) {
        strategies.push_back(std::make_unique<LoraStrategy>());
        LOG_INFO("factory", "Created LoRa strategy");
    }

    if (CFG_INT("communication", "enable_wifi", 1)) {
        strategies.push_back(std::make_unique<WifiStrategy>());
        LOG_INFO("factory", "Created WiFi strategy");
    }

    if (CFG_INT("communication", "enable_bluetooth", 1)) {
        strategies.push_back(std::make_unique<BluetoothStrategy>());
        LOG_INFO("factory", "Created Bluetooth strategy");
    }

    if (CFG_INT("communication", "enable_lan", 1)) {
        strategies.push_back(std::make_unique<LanStrategy>());
        LOG_INFO("factory", "Created LAN strategy");
    }

    if (CFG_INT("communication", "enable_lan_server", 1)) {
        strategies.push_back(std::make_unique<LanServerStrategy>());
        LOG_INFO("factory", "Created LAN Server strategy");
    }

    if (CFG_INT("communication", "enable_4g", 0)) {
        strategies.push_back(std::make_unique<Cell4GStrategy>());
        LOG_INFO("factory", "Created 4G strategy");
    }

    LOG_INFO("factory", "Created %zu communication strategies", strategies.size());
    return strategies;
}

std::unique_ptr<ICommunicationStrategy> CommunicationFactory::create(const std::string& type) {
    if (type == "lora") {
        return std::make_unique<LoraStrategy>();
    } else if (type == "wifi") {
        return std::make_unique<WifiStrategy>();
    } else if (type == "bt" || type == "bluetooth") {
        return std::make_unique<BluetoothStrategy>();
    } else if (type == "lan") {
        return std::make_unique<LanStrategy>();
    } else if (type == "lan_server") {
        return std::make_unique<LanServerStrategy>();
    } else if (type == "4g") {
        return std::make_unique<Cell4GStrategy>();
    } else {
        LOG_ERROR("factory", "Unknown communication type: %s", type.c_str());
        return nullptr;
    }
}

} // namespace sap
