/**
 * @file cell4g_strategy.h
 * @brief 4G 蜂窝网络通信策略
 */
#pragma once

#include "core/communication_strategy.h"
#include "infra/logger.h"
#include "infra/config.h"
#include "hal/de_cell4g.h"
#include <unistd.h>

namespace sap {

/**
 * @brief 4G 通信策略实现
 */
class Cell4GStrategy : public CommunicationStrategyBase {
public:
    int initialize() override {
        std::string server_ip = CFG_STR("network.cell4g", "server_ip", "106.52.84.156");
        int server_port = CFG_INT("network.cell4g", "server_port", 8888);

        fd_ = cell4g_ip_port_check(
            const_cast<char*>(server_ip.c_str()), server_port);

        if (fd_ >= 0) {
            LOG_INFO("cell4g", "4G connected to %s:%d, fd=%d",
                    server_ip.c_str(), server_port, fd_);
        } else {
            LOG_ERROR("cell4g", "4G connection failed");
        }
        return fd_;
    }

    int reinitialize(int old_fd) override {
        close(old_fd);
        fd_ = -1;
        return initialize();
    }

    std::string typeName() const override { return "4G"; }
    int deviceId() const override { return 6; }
};

} // namespace sap
