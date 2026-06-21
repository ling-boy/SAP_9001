/**
 * @file bt_strategy.h
 * @brief 蓝牙通信策略
 */
#pragma once

#include "core/communication_strategy.h"
#include "infra/logger.h"
#include "hal/bluetooth.h"
#include <cstdlib>

namespace sap {

/**
 * @brief 蓝牙通信策略实现
 */
class BluetoothStrategy : public CommunicationStrategyBase {
public:
    int initialize() override {
        fd_ = bluetooth_open(mac_);
        if (fd_ >= 0) {
            // 点亮绿色 LED
            system("echo 1 > /sys/class/leds/green/brightness");
            LOG_INFO("bluetooth", "Bluetooth initialized, fd=%d, mac=%s", fd_, mac_.c_str());
        } else {
            LOG_ERROR("bluetooth", "%s", "Bluetooth initialization failed");
        }
        return fd_;
    }

    int reinitialize(int old_fd) override {
        (void)old_fd;  // caller is responsible for closing old_fd
        fd_ = -1;
        return initialize();
    }

    std::string typeName() const override { return "Bluetooth"; }
    int deviceId() const override { return 3; }

    /**
     * @brief 获取扫描到的设备 MAC
     */
    const std::string& getDeviceMac() const { return mac_; }

private:
    std::string mac_;
};

} // namespace sap
