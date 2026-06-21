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
#include "protocol/protocol_process.h"
#include "infra/get.h"
#include "infra/logger.h"
#include "hal/gps.h"

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
        // 使用线程安全的 getter 方法
        device_id_ = ctx.getIdentityId();
        net_id_ = ctx.getIdentityNetId();
        mac_ = ctx.getIdentityMac();
        isr_mac_ = ctx.getIdentityIsrMac();
        comm_status_ = ctx.getCommunicateStatus();
        cpu_mem_ = get_cpuOccupy();
        gps_ = gps_get_location();

        return *this;
    }

    /**
     * @brief 构建协议包
     * @return 完整的 06 协议包字符串
     */
    std::string build() const {
        auto& ctx = DeviceContext::instance();

        // 获取通信类型（处理负数设备ID）
        int activeId = ctx.getActiveDeviceId();
        std::string communicateType = (activeId >= 0) ? std::to_string(activeId) : "0";

        // 计算长度（不含 gps_ 字段，GPS 数据由传感器数据段携带）
        int length = data_.length() + 43;
        std::string len_str = "";
        change(length, len_str);

        // 确保长度字段 4 位
        int num = len_str.length();
        if (num == 1) len_str = "000" + len_str;
        else if (num == 2) len_str = "00" + len_str;
        else if (num == 3) len_str = "0" + len_str;

        // 从内存池分配缓冲区（O(1) 操作，避免堆碎片）
        auto* buf = ctx.packetPool().allocate();
        if (buf == nullptr) {
            LOG_ERROR("hj212", "%s", "Packet pool exhausted");
            return "";
        }
        char* packet = buf->data;

        // 组装协议包
        int needed = snprintf(packet, 2048,
            "$%s%s%s%s00%s%s%s%s%s%s%s@",
            command_.c_str(), communicateType.c_str(), device_id_.c_str(), net_id_.c_str(),
            len_str.c_str(), isr_mac_.c_str(), mac_.c_str(),
            port_info_.c_str(), cpu_mem_.c_str(),
            comm_status_.c_str(), data_.c_str());

        if (needed < 0) {
            ctx.packetPool().deallocate(buf);
            return "";
        }
        if (needed >= 2048) {
            LOG_ERROR("hj212", "Packet too large (%d bytes), truncating", needed);
        }

        // 复制到 string
        std::string result(packet);

        // 归还缓冲区到内存池
        ctx.packetPool().deallocate(buf);

        return result;
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

    // 使用 protocol_process.h 中的全局 change() 和 switch10_16() 函数
};

} // namespace sap
