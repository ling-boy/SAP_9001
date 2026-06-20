/**
 * @file event_bus.h
 * @brief 事件总线（观察者模式）
 * @details 线程安全的发布-订阅事件系统，用于解耦模块间通信
 *
 * 使用示例：
 *   // 订阅事件
 *   int id = EventBus::instance().subscribe(EventType::DataRequestReceived,
 *       [](const EventData& e) { handleRequest(e); });
 *
 *   // 发布事件
 *   EventBus::instance().publish(EventType::SensorDataReady, data);
 *
 *   // 取消订阅
 *   EventBus::instance().unsubscribe(id);
 *
 * @version 1.0
 */
#pragma once

#include <functional>
#include <vector>
#include <mutex>
#include <map>
#include <string>
#include "infra/logger.h"

namespace sap {

/**
 * @brief 事件类型枚举
 */
enum class EventType {
    SensorDataReady,        // 传感器数据就绪
    DataRequestReceived,    // 收到数据请求（03协议）
    RegistrationSuccess,    // 设备注册成功
    ConnectionEstablished,  // 连接建立
    ConnectionLost,         // 连接断开
    TimeSyncReceived,       // 收到时间同步
    WatchdogTimeout,        // 看门狗超时
    ShutdownRequested       // 关机请求
};

/**
 * @brief 事件类型名称（用于日志）
 */
inline const char* eventTypeName(EventType type) {
    switch (type) {
        case EventType::SensorDataReady:      return "SensorDataReady";
        case EventType::DataRequestReceived:   return "DataRequestReceived";
        case EventType::RegistrationSuccess:   return "RegistrationSuccess";
        case EventType::ConnectionEstablished: return "ConnectionEstablished";
        case EventType::ConnectionLost:        return "ConnectionLost";
        case EventType::TimeSyncReceived:      return "TimeSyncReceived";
        case EventType::WatchdogTimeout:       return "WatchdogTimeout";
        case EventType::ShutdownRequested:     return "ShutdownRequested";
        default:                               return "Unknown";
    }
}

/**
 * @brief 事件数据基类
 */
struct EventData {
    virtual ~EventData() = default;
};

/**
 * @brief 传感器数据就绪事件
 */
struct SensorDataEvent : public EventData {
    bool is_ship;           // true=船载传感器, false=大气传感器
    std::string data;       // HJ212 数据包
};

/**
 * @brief 数据请求事件
 */
struct DataRequestEvent : public EventData {
    std::string mac;                // 请求方 MAC
    std::string communicate_method; // 通信方式
};

/**
 * @brief 时间同步事件
 */
struct TimeSyncEvent : public EventData {
    std::string timestamp;  // 时间戳字符串
};

/**
 * @brief 连接状态事件
 */
struct ConnectionEvent : public EventData {
    int device_id;          // 设备 ID
    int fd;                 // 文件描述符
    std::string reason;     // 原因
};

/**
 * @brief 事件总线（观察者模式，单例）
 * @details 线程安全的发布-订阅事件系统
 */
class EventBus {
public:
    /**
     * @brief 获取单例实例
     */
    static EventBus& instance() {
        static EventBus inst;
        return inst;
    }

    /**
     * @brief 订阅事件
     * @param type 事件类型
     * @param handler 事件处理函数
     * @return 订阅 ID（用于取消订阅）
     */
    int subscribe(EventType type, std::function<void(const EventData&)> handler) {
        std::lock_guard<std::mutex> lock(mtx_);
        int id = next_id_++;
        handlers_[type].push_back({id, std::move(handler)});
        LOG_DEBUG("event_bus", "Subscribed to %s, id=%d", eventTypeName(type), id);
        return id;
    }

    /**
     * @brief 取消订阅
     * @param subscriptionId 订阅 ID
     */
    void unsubscribe(int subscriptionId) {
        std::lock_guard<std::mutex> lock(mtx_);
        for (auto& [type, handlers] : handlers_) {
            handlers.erase(
                std::remove_if(handlers.begin(), handlers.end(),
                    [subscriptionId](const auto& h) { return h.first == subscriptionId; }),
                handlers.end());
        }
        LOG_DEBUG("event_bus", "Unsubscribed id=%d", subscriptionId);
    }

    /**
     * @brief 发布事件（同步调用所有订阅者）
     * @param type 事件类型
     * @param data 事件数据
     */
    void publish(EventType type, const EventData& data = EventData{}) {
        std::vector<std::function<void(const EventData&)>> handlers_copy;
        {
            std::lock_guard<std::mutex> lock(mtx_);
            auto it = handlers_.find(type);
            if (it != handlers_.end()) {
                for (auto& [id, handler] : it->second) {
                    handlers_copy.push_back(handler);
                }
            }
        }
        // 在锁外调用处理器，避免死锁
        for (auto& handler : handlers_copy) {
            try {
                handler(data);
            } catch (const std::exception& e) {
                LOG_ERROR("event_bus", "Handler exception for %s: %s",
                         eventTypeName(type), e.what());
            }
        }
    }

    /**
     * @brief 获取订阅者数量
     */
    size_t subscriberCount(EventType type) const {
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = handlers_.find(type);
        return it != handlers_.end() ? it->second.size() : 0;
    }

    // 禁止拷贝
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;

private:
    EventBus() = default;

    mutable std::mutex mtx_;
    std::map<EventType, std::vector<std::pair<int, std::function<void(const EventData&)>>>> handlers_;
    int next_id_ = 0;
};

} // namespace sap
