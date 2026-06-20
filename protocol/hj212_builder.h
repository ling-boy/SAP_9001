/**
 * @file hj212_builder.h
 * @brief HJ212 协议包建造者（建造者模式）
 * @details 使用流式 API 构建 HJ212 协议包，简化 packet06() 函数
 *
 * 使用示例：
 *   auto packet = HJ212PacketBuilder()
 *       .fromContext()           // 自动从 DeviceContext 填充设备信息
 *       .withData(sensor_data)
 *       .withPortInfo("100")
 *       .build();
 *
 * @version 1.0
 */
#pragma once

#include <string>
#include "core/device_context.h"
#include "protocol/hj212.h"
#include "infra/get.h"
#include "infra/logger.h"

namespace sap {

/**
 * @brief HJ212 协议包建造者
 */
class HJ212PacketBuilder {
public:
    /**
     * @brief 设置协议命令号
     */
    HJ212PacketBuilder& withCommand(const std::string& cmd) {
        command_ = cmd;
        return *this;
    }

    /**
     * @brief 设置设备 ID
     */
    HJ212PacketBuilder& withDeviceId(const std::string& id) {
        device_id_ = id;
        return *this;
    }

    /**
     * @brief 设置网络 ID
     */
    HJ212PacketBuilder& withNetId(const std::string& net_id) {
        net_id_ = net_id;
        return *this;
    }

    /**
     * @brief 设置设备 MAC
     */
    HJ212PacketBuilder& withMac(const std::string& mac) {
        mac_ = mac;
        return *this;
    }

    /**
     * @brief 设置 ISR MAC
     */
    HJ212PacketBuilder& withIsrMac(const std::string& isr_mac) {
        isr_mac_ = isr_mac;
        return *this;
    }

    /**
     * @brief 设置 GPS 坐标
     */
    HJ212PacketBuilder& withGPS(const std::string& gps) {
        gps_ = gps;
        return *this;
    }

    /**
     * @brief 设置 CPU/内存信息
     */
    HJ212PacketBuilder& withCpuMem(const std::string& cpu_mem) {
        cpu_mem_ = cpu_mem;
        return *this;
    }

    /**
     * @brief 设置通信状态
     */
    HJ212PacketBuilder& withCommStatus(const std::string& status) {
        comm_status_ = status;
        return *this;
    }

    /**
     * @brief 设置数据内容
     */
    HJ212PacketBuilder& withData(const std::string& data) {
        data_ = data;
        return *this;
    }

    /**
     * @brief 设置端口信息
     */
    HJ212PacketBuilder& withPortInfo(const std::string& port_info) {
        port_info_ = port_info;
        return *this;
    }

    /**
     * @brief 从 DeviceContext 自动填充设备信息
     */
    HJ212PacketBuilder& fromContext() {
        auto& ctx = DeviceContext::instance();
        auto& id = ctx.identity();

        device_id_ = id.id;
        net_id_ = id.net_id;
        mac_ = id.mac;
        isr_mac_ = id.isr_mac;
        comm_status_ = id.communicate_status;
        cpu_mem_ = get_cpuOccupy();
        gps_ = "N2932E10636";  // 默认 GPS，实际应从 GPS 模块获取

        return *this;
    }

    /**
     * @brief 构建协议包
     * @return 完整的 06 协议包字符串
     */
    std::string build() const {
        auto& ctx = DeviceContext::instance();

        // 获取通信类型
        std::string communicateType = std::to_string(ctx.getActiveDeviceId());

        // 计算长度
        int length = data_.length() + 54;
        std::string len_str = "";
        change(length, len_str);

        // 确保长度字段 4 位
        int num = len_str.length();
        if (num == 1) len_str = "000" + len_str;
        else if (num == 2) len_str = "00" + len_str;
        else if (num == 3) len_str = "0" + len_str;

        // 组装协议包（使用 2048 字节栈缓冲区，避免堆分配）
        char packet[2048] = {0};
        int needed = snprintf(packet, sizeof(packet),
            "$06%s%s%s00%s%s%s%s%s%s%s%s@",
            communicateType.c_str(), device_id_.c_str(), net_id_.c_str(),
            len_str.c_str(), isr_mac_.c_str(), mac_.c_str(),
            port_info_.c_str(), gps_.c_str(), cpu_mem_.c_str(),
            comm_status_.c_str(), data_.c_str());

        if (needed < 0) return "";
        if (needed >= (int)sizeof(packet)) {
            LOG_ERROR("hj212", "Packet too large (%d bytes), truncating", needed);
        }
        return std::string(packet);
    }

private:
    std::string command_ = "06";
    std::string device_id_ = "FF";
    std::string net_id_ = "0000";
    std::string mac_;
    std::string isr_mac_;
    std::string gps_ = "N2932E10636";
    std::string cpu_mem_;
    std::string comm_status_ = "0000";
    std::string data_;
    std::string port_info_;

    /**
     * @brief 十进制转十六进制字符串
     */
    static void change(long long int num, std::string& str) {
        if (num < 16) {
            str += switch10_16(num);
            return;
        }
        change(num / 16, str);
        str += switch10_16(num % 16);
    }

    static std::string switch10_16(int num) {
        switch(num) {
            case 0: return "0"; case 1: return "1"; case 2: return "2";
            case 3: return "3"; case 4: return "4"; case 5: return "5";
            case 6: return "6"; case 7: return "7"; case 8: return "8";
            case 9: return "9"; case 10: return "A"; case 11: return "B";
            case 12: return "C"; case 13: return "D"; case 14: return "E";
            case 15: return "F"; default: return "0";
        }
    }
};

} // namespace sap
