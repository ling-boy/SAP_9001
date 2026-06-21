/**
 * @file device_context.h
 * @brief 设备上下文单例 - 集中管理所有全局状态
 * @details 替代 modbus_9001.cpp 中的 30+ 个全局变量
 *          使用单例模式确保全局唯一访问点
 *
 * 使用示例：
 *   auto& ctx = DeviceContext::instance();
 *   ctx.identity().mac = "AABBCCDD";
 *   ctx.fds().lora = lora_fd;
 *   ctx.queues().transmit.push(packet);
 *
 * @version 1.0
 */
#pragma once

#include <string>
#include <vector>
#include <queue>
#include <cstring>
#include <pthread.h>
#include "infra/communica_manage.h"
#include "infra/message_queue.h"
#include "infra/software_wdt.h"
#include "infra/memory_pool.h"

namespace sap {

/**
 * @brief 设备上下文单例
 * @details 集中管理设备标识、通信FD、线程同步、消息队列、看门狗等全局状态
 */
class DeviceContext {
public:
    /**
     * @brief 获取单例实例
     */
    static DeviceContext& instance() {
        static DeviceContext inst;
        return inst;
    }

    // ======================================================================
    // 设备标识
    // ======================================================================
    struct DeviceIdentity {
        std::string id = "FF";
        std::string net_id = "0000";
        std::string mac;
        std::string bt_mac;
        std::string isr_mac;
        std::string current_time;
        std::string cpu_occupy;
        char communicate_status[6] = "0000";  // 增大到6，防止id=5越界
        int monitor_time = 4;
    };

    // ======================================================================
    // 通信设备文件描述符
    // ======================================================================
    struct CommFds {
        int lora = -1;
        int wifi = -1;
        int bt = -1;
        int lan = -1;
        int lan_server = -1;
        int flag_wifi = 0;
        bool gps_failed = false;                 // GPS 初始化失败标志
        std::vector<int> device_id;              // 已初始化成功的通信设备ID列表
    };

    // ======================================================================
    // 线程同步（传感器数据就绪通知）
    // ======================================================================
    struct SensorSync {
        pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
        pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
        int ship_data_ready = 0;
        int gas_data_ready = 0;
    };

    // ======================================================================
    // 消息队列
    // ======================================================================
    struct MessageQueues {
        MessageQueue<std::string> transmit;      // 实时发送队列
        std::queue<std::string> offline_cache;   // 离线缓存队列
        pthread_mutex_t offline_mtx = PTHREAD_MUTEX_INITIALIZER;  // 离线缓存互斥锁
        unsigned int hj_crc = 0;                 // HJ212 CRC 校验值
    };

    // ======================================================================
    // 看门狗状态
    // ======================================================================
    struct WatchdogState {
        int ship_wdt_id = -1;
        int gas_wdt_id = -1;
        int timeout_sec = 30;
    };

    // ======================================================================
    // 线程 ID
    // ======================================================================
    struct ThreadIds {
        pthread_t gps = 0;
        pthread_t device_regist = 0;
        pthread_t watchdog = 0;
        pthread_t ship_data = 0;
        pthread_t trans_message = 0;
        pthread_t get_sensor = 0;
        pthread_t get_lan = 0;
        pthread_t monitor_485 = 0;
    };

    // ======================================================================
    // 访问接口
    // ======================================================================
    DeviceIdentity& identity() { return identity_; }
    const DeviceIdentity& identity() const { return identity_; }

    CommFds& fds() { return fds_; }
    const CommFds& fds() const { return fds_; }

    SensorSync& sensorSync() { return sensor_sync_; }

    MessageQueues& queues() { return queues_; }

    WatchdogState& watchdog() { return watchdog_; }

    ThreadIds& threads() { return threads_; }

    communicaManage& commManager() { return comm_mgr_; }
    CSoftwareWdt& softwareWdt() { return wdt_; }

    /**
     * @brief 06包缓冲区结构体
     */
    struct PacketBuffer {
        char data[2048];
    };

    /**
     * @brief 获取06包内存池
     * @details 预分配 8 个 2048 字节缓冲区，用于06包构建
     */
    MemoryPool<PacketBuffer, 8>& packetPool() { return packet_pool_; }

    // ======================================================================
    // 便捷方法
    // ======================================================================

    /**
     * @brief 获取第一个已注册成功设备的 ID
     */
    int getActiveDeviceId() {
        return comm_mgr_.getSuccessId();
    }

    /**
     * @brief 向发送队列推送数据
     */
    void pushTransmit(const std::string& data) {
        queues_.transmit.push(data);
    }

    /**
     * @brief 通知传感器数据就绪
     * @param is_ship true=船载传感器, false=大气传感器
     */
    void notifySensorDataReady(bool is_ship) {
        pthread_mutex_lock(&sensor_sync_.mtx);
        if (is_ship) {
            sensor_sync_.ship_data_ready = 1;
        } else {
            sensor_sync_.gas_data_ready = 1;
        }
        pthread_mutex_unlock(&sensor_sync_.mtx);
        pthread_cond_broadcast(&sensor_sync_.cond);
    }

    /**
     * @brief 等待传感器数据就绪
     * @param is_ship true=船载传感器, false=大气传感器
     */
    void waitForSensorData(bool is_ship) {
        pthread_mutex_lock(&sensor_sync_.mtx);
        if (is_ship) {
            while (sensor_sync_.ship_data_ready == 0)
                pthread_cond_wait(&sensor_sync_.cond, &sensor_sync_.mtx);
            sensor_sync_.ship_data_ready = 0;
        } else {
            while (sensor_sync_.gas_data_ready == 0)
                pthread_cond_wait(&sensor_sync_.cond, &sensor_sync_.mtx);
            sensor_sync_.gas_data_ready = 0;
        }
        pthread_mutex_unlock(&sensor_sync_.mtx);
    }

    /**
     * @brief 线程安全地设置设备标识
     * @details 防止多线程并发读写 std::string 导致未定义行为
     */
    void setIdentityId(const std::string& val) { std::lock_guard<std::mutex> lock(identity_mtx_); identity_.id = val; }
    void setIdentityNetId(const std::string& val) { std::lock_guard<std::mutex> lock(identity_mtx_); identity_.net_id = val; }
    void setIdentityMac(const std::string& val) { std::lock_guard<std::mutex> lock(identity_mtx_); identity_.mac = val; }
    void setIdentityBtMac(const std::string& val) { std::lock_guard<std::mutex> lock(identity_mtx_); identity_.bt_mac = val; }
    void setIdentityIsrMac(const std::string& val) { std::lock_guard<std::mutex> lock(identity_mtx_); identity_.isr_mac = val; }
    void setIdentityCurrentTime(const std::string& val) { std::lock_guard<std::mutex> lock(identity_mtx_); identity_.current_time = val; }
    void setIdentityCpuOccupy(const std::string& val) { std::lock_guard<std::mutex> lock(identity_mtx_); identity_.cpu_occupy = val; }

    /**
     * @brief 线程安全地设置 communicate_status
     * @param index 数组索引（0-5）
     * @param val   字符值（'0' 或 '1'）
     */
    void setCommunicateStatus(int index, char val) {
        if (index >= 0 && index < 6) {
            std::lock_guard<std::mutex> lock(identity_mtx_);
            identity_.communicate_status[index] = val;
        }
    }

    /**
     * @brief 线程安全地获取 communicate_status
     * @return 状态字符串的副本
     */
    std::string getCommunicateStatus() const {
        std::lock_guard<std::mutex> lock(identity_mtx_);
        return std::string(identity_.communicate_status);
    }

    /**
     * @brief 线程安全地获取设备标识
     */
    std::string getIdentityId() const { std::lock_guard<std::mutex> lock(identity_mtx_); return identity_.id; }
    std::string getIdentityNetId() const { std::lock_guard<std::mutex> lock(identity_mtx_); return identity_.net_id; }
    std::string getIdentityMac() const { std::lock_guard<std::mutex> lock(identity_mtx_); return identity_.mac; }
    std::string getIdentityIsrMac() const { std::lock_guard<std::mutex> lock(identity_mtx_); return identity_.isr_mac; }
    std::string getIdentityCurrentTime() const { std::lock_guard<std::mutex> lock(identity_mtx_); return identity_.current_time; }
    std::string getIdentityCpuOccupy() const { std::lock_guard<std::mutex> lock(identity_mtx_); return identity_.cpu_occupy; }

    // 禁止拷贝
    DeviceContext(const DeviceContext&) = delete;
    DeviceContext& operator=(const DeviceContext&) = delete;

private:
    DeviceContext() = default;

    DeviceIdentity identity_;
    mutable std::mutex identity_mtx_;  // 保护 DeviceIdentity 的并发访问
    CommFds fds_;
    SensorSync sensor_sync_;
    MessageQueues queues_;
    WatchdogState watchdog_;
    ThreadIds threads_;
    communicaManage comm_mgr_;
    CSoftwareWdt wdt_;
    MemoryPool<PacketBuffer, 8> packet_pool_;  // 06包内存池（8个缓冲区）
};

} // namespace sap
