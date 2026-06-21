/**
 * @file wifi_strategy.h
 * @brief WiFi 通信策略
 * @note **头文件循环依赖风险**：本文件 include 了 "app/device_init.h"，
 *       而 device_init.h 可能间接 include 本文件的同级头文件。
 *       当前通过 #pragma once 和前向声明避免了实际循环，
 *       但若未来 WifiStrategy 需要访问更多 app 层接口，
 *       建议将 WiFi 初始化逻辑移至 .cpp 文件，仅在 .h 中声明接口。
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
 * @brief WiFi 通信策略实现
 * @details 包含完整的 WiFi 初始化逻辑：wpa_supplicant、udhcpc、socket connect
 */
class WifiStrategy : public CommunicationStrategyBase {
public:
    int initialize() override {
        // 1. 关闭 wlan0
        LOG_INFO("wifi", "%s", "Bringing down wlan0...");
        system("ifconfig wlan0 down");
        sleep(2);

        // 2. 杀死已有的 wpa_supplicant 和 udhcpc
        system("killall wpa_supplicant");
        sleep(1);
        system("killall udhcpc");
        sleep(1);

        // 3. 启动 wpa_supplicant
        std::string wifi_conf = CFG_STR("paths", "wifi_conf", "/home/root/wifi.conf");
        std::string wpa_cmd = "wpa_supplicant -i wlan0 -B -c " + wifi_conf + " &";
        LOG_INFO("wifi", "Starting wpa_supplicant with config: %s", wifi_conf.c_str());
        system(wpa_cmd.c_str());
        sleep(10);

        // 4. 启动 DHCP 客户端
        LOG_INFO("wifi", "%s", "Starting udhcpc...");
        system("udhcpc -i wlan0 -t 8 -n");
        sleep(5);

        // 5. 创建 socket 并连接
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

        // 6. 点亮黄色 LED
        system("echo 1 > /sys/class/leds/yellow/brightness");

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
