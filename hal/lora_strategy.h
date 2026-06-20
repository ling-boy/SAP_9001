/**
 * @file lora_strategy.h
 * @brief LoRa 通信策略
 */
#pragma once

#include "core/communication_strategy.h"
#include "infra/logger.h"
#include "hal/lora.h"
#include <cstdlib>

namespace sap {

/**
 * @brief LoRa 通信策略实现
 */
class LoraStrategy : public CommunicationStrategyBase {
public:
    int initialize() override {
        fd_ = lora_open();
        if (fd_ >= 0) {
            // 点亮红色 LED
            system("echo 1 > /sys/class/leds/red/brightness");
            LOG_INFO("lora", "LoRa initialized, fd=%d", fd_);
        } else {
            LOG_ERROR("lora", "%s", "LoRa initialization failed");
        }
        return fd_;
    }

    int reinitialize(int old_fd) override {
        close(old_fd);
        fd_ = lora_open();
        if (fd_ >= 0) {
            LOG_INFO("lora", "LoRa reinitialized, fd=%d", fd_);
        } else {
            LOG_ERROR("lora", "%s", "LoRa reinitialization failed");
        }
        return fd_;
    }

    std::string typeName() const override { return "LoRa"; }
    int deviceId() const override { return 1; }
};

} // namespace sap
