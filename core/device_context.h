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
#include <mutex>
#include <condition_variable>
#include <cstring>
#include "infra/communica_manage.h"
#include "infra/message_queue.h"
#include "infra/software_wdt.h"

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
        char communicate_status[5] = "0000";
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
    };

    // ======================================================================
    // 线程同步（传感器数据就绪通知）
    // ======================================================================
    struct SensorSync {
        std::mutex mtx;
        std::condition_variable cond;
        int ship_data_ready = 0;
        int gas_data_ready = 0;
    };

    // ======================================================================
    // 消息队列
    // ======================================================================
    struct MessageQueues {
        MessageQueue<std::string> transmit;      // 实时发送队列
        std::queue<std::string> offline_cache;   // 离线缓存队列
        std::mutex offline_mtx;                   // 离线缓存互斥锁
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

    // ======================================================================
    // 便捷方法
    // ======================================================================

    /**
     * @brief 获取第一个已注册成功设备的 fd
     */
    int getActiveFd() {
        return comm_mgr_.getSuccessFd();
    }

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
        std::lock_guard<std::mutex> lock(sensor_sync_.mtx);
        if (is_ship) {
            sensor_sync_.ship_data_ready = 1;
        } else {
            sensor_sync_.gas_data_ready = 1;
        }
        sensor_sync_.cond.notify_all();
    }

    /**
     * @brief 等待传感器数据就绪
     * @param is_ship true=船载传感器, false=大气传感器
     */
    void waitForSensorData(bool is_ship) {
        std::unique_lock<std::mutex> lock(sensor_sync_.mtx);
        if (is_ship) {
            sensor_sync_.cond.wait(lock, [this]{ return sensor_sync_.ship_data_ready != 0; });
            sensor_sync_.ship_data_ready = 0;
        } else {
            sensor_sync_.cond.wait(lock, [this]{ return sensor_sync_.gas_data_ready != 0; });
            sensor_sync_.gas_data_ready = 0;
        }
    }

    // 禁止拷贝
    DeviceContext(const DeviceContext&) = delete;
    DeviceContext& operator=(const DeviceContext&) = delete;

private:
    DeviceContext() = default;

    DeviceIdentity identity_;
    CommFds fds_;
    SensorSync sensor_sync_;
    MessageQueues queues_;
    WatchdogState watchdog_;
    ThreadIds threads_;
    communicaManage comm_mgr_;
    CSoftwareWdt wdt_;
};

} // namespace sap
