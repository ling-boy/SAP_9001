/**
 * @file wifi_strategy.h
 * @brief WiFi 通信策略
 */
#pragma once

#include "core/communication_strategy.h"
#include "infra/logger.h"
#include "infra/config.h"
#include "app/device_init.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

namespace sap {

/**
 * @brief WiFi 通信策略实现
 */
class WifiStrategy : public CommunicationStrategyBase {
public:
    int initialize() override {
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ < 0) {
            LOG_ERROR("wifi", "Socket create failed: %s", strerror(errno));
            return -1;
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(cfg_wifi_port());
        addr.sin_addr.s_addr = inet_addr(cfg_wifi_ip().c_str());

        int ret = connect_nonb(fd_, (struct sockaddr*)&addr, sizeof(addr), cfg_connect_timeout());
        if (ret < 0) {
            LOG_ERROR("wifi", "Connect to %s:%d failed: %s",
                     cfg_wifi_ip().c_str(), cfg_wifi_port(), strerror(errno));
            close(fd_);
            fd_ = -1;
            return -1;
        }

        LOG_INFO("wifi", "WiFi connected to %s:%d, fd=%d",
                cfg_wifi_ip().c_str(), cfg_wifi_port(), fd_);
        return fd_;
    }

    int reinitialize(int old_fd) override {
        close(old_fd);
        fd_ = -1;
        return initialize();
    }

    std::string typeName() const override { return "WiFi"; }
    int deviceId() const override { return 2; }
};

} // namespace sap
