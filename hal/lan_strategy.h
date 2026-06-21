/**
 * @file lan_strategy.h
 * @brief LAN 有线通信策略
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
#include <cstdlib>
#include <cerrno>

namespace sap {

/**
 * @brief LAN 有线通信策略实现（客户端模式，连接 ISR）
 */
class LanStrategy : public CommunicationStrategyBase {
public:
    int initialize() override {
        // 1. 配置 eth1 IP 地址
        std::string eth1_ip = CFG_STR("network.lan", "local_ip", "192.168.2.235");
        std::string ifconfig_cmd = "ifconfig eth1 " + eth1_ip + " netmask 255.255.255.0";
        LOG_INFO("lan", "Configuring eth1: %s", eth1_ip.c_str());
        system(ifconfig_cmd.c_str());
        sleep(2);

        // 2. 创建 socket
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ < 0) {
            LOG_ERROR("lan", "Socket create failed: %s", strerror(errno));
            return -1;
        }

        // 3. 连接 ISR
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(cfg_lan_ip().c_str());
        addr.sin_port = htons(cfg_lan_port());

        // 使用非阻塞 connect
        int ret = connect_nonb(fd_, (struct sockaddr*)&addr, sizeof(addr), cfg_connect_timeout());
        if (ret < 0) {
            LOG_ERROR("lan", "Connect to %s:%d failed: %s",
                     cfg_lan_ip().c_str(), cfg_lan_port(), strerror(errno));
            close(fd_);
            fd_ = -1;
            return -1;
        }

        LOG_INFO("lan", "LAN connected to %s:%d, fd=%d",
                cfg_lan_ip().c_str(), cfg_lan_port(), fd_);
        return fd_;
    }

    int reinitialize(int old_fd) override {
        (void)old_fd;  // caller is responsible for closing old_fd
        fd_ = -1;
        return initialize();
    }

    std::string typeName() const override { return "LAN"; }
    int deviceId() const override { return 4; }
};

/**
 * @brief LAN 服务器端策略（接收其他节点数据）
 * @note 当前实现仅创建监听 socket，accept() 逻辑尚未实现
 *       预留接口供后续扩展：接收其他监测节点的数据
 */
class LanServerStrategy : public CommunicationStrategyBase {
public:
    int initialize() override {
        // 1. 配置 eth0 IP 地址
        std::string eth0_ip = CFG_STR("network.lan", "zd_local_ip", "192.168.31.101");
        std::string ifconfig_cmd = "ifconfig eth0 " + eth0_ip + " netmask 255.255.255.0";
        LOG_INFO("lan_server", "Configuring eth0: %s", eth0_ip.c_str());
        system(ifconfig_cmd.c_str());
        sleep(2);

        // 2. 创建 socket
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ < 0) {
            LOG_ERROR("lan_server", "Socket create failed: %s", strerror(errno));
            return -1;
        }

        // 3. 绑定地址
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(cfg_zd_lan_ip().c_str());
        addr.sin_port = htons(cfg_zd_lan_port());

        if (bind(fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            LOG_ERROR("lan_server", "Bind failed: %s", strerror(errno));
            close(fd_);
            fd_ = -1;
            return -1;
        }

        // 4. 开始监听
        if (listen(fd_, 5) < 0) {
            LOG_ERROR("lan_server", "Listen failed: %s", strerror(errno));
            close(fd_);
            fd_ = -1;
            return -1;
        }

        LOG_INFO("lan_server", "LAN server listening on %s:%d, fd=%d",
                cfg_zd_lan_ip().c_str(), cfg_zd_lan_port(), fd_);
        return fd_;
    }

    int reinitialize(int old_fd) override {
        (void)old_fd;  // caller is responsible for closing old_fd
        fd_ = -1;
        return initialize();
    }

    std::string typeName() const override { return "LAN_Server"; }
    int deviceId() const override { return 5; }
    bool isClient() const override { return false; }  // 服务器监听 socket
};

} // namespace sap
