/**
 * @file device_state_machine.h
 * @brief 设备状态机（状态机模式）
 * @details 管理设备生命周期：初始化 → 注册 → 运行 → 错误 → 恢复
 *
 * 使用示例：
 *   DeviceStateMachine sm;
 *   sm.onStateChange(DeviceState::Running, []() {
 *       LOG_INFO("main", "Device is now running");
 *   });
 *   sm.handleEvent(DeviceEvent::RegisterSuccess);
 *
 * @version 1.0
 */
#pragma once

#include <functional>
#include <map>
#include <vector>
#include <string>
#include "infra/logger.h"

namespace sap {

/**
 * @brief 设备状态枚举
 */
enum class DeviceState {
    Uninitialized,  // 未初始化
    Initializing,   // 初始化中
    Registering,    // 注册中
    Running,        // 正常运行
    Error,          // 错误状态
    Recovering      // 恢复中
};

/**
 * @brief 设备事件枚举
 */
enum class DeviceEvent {
    InitStart,          // 开始初始化
    InitSuccess,        // 初始化成功
    InitFailed,         // 初始化失败
    RegisterStart,      // 开始注册
    RegisterSuccess,    // 注册成功
    RegisterFailed,     // 注册失败
    ConnectionLost,     // 连接断开
    RecoveryStart,      // 开始恢复
    RecoverySuccess,    // 恢复成功
    ErrorOccurred       // 发生错误
};

/**
 * @brief 状态名称（用于日志）
 */
inline const char* stateName(DeviceState state) {
    switch (state) {
        case DeviceState::Uninitialized: return "Uninitialized";
        case DeviceState::Initializing:  return "Initializing";
        case DeviceState::Registering:   return "Registering";
        case DeviceState::Running:       return "Running";
        case DeviceState::Error:         return "Error";
        case DeviceState::Recovering:    return "Recovering";
        default:                         return "Unknown";
    }
}

/**
 * @brief 事件名称（用于日志）
 */
inline const char* eventName(DeviceEvent event) {
    switch (event) {
        case DeviceEvent::InitStart:       return "InitStart";
        case DeviceEvent::InitSuccess:     return "InitSuccess";
        case DeviceEvent::InitFailed:      return "InitFailed";
        case DeviceEvent::RegisterStart:   return "RegisterStart";
        case DeviceEvent::RegisterSuccess: return "RegisterSuccess";
        case DeviceEvent::RegisterFailed:  return "RegisterFailed";
        case DeviceEvent::ConnectionLost:  return "ConnectionLost";
        case DeviceEvent::RecoveryStart:   return "RecoveryStart";
        case DeviceEvent::RecoverySuccess: return "RecoverySuccess";
        case DeviceEvent::ErrorOccurred:   return "ErrorOccurred";
        default:                           return "Unknown";
    }
}

/**
 * @brief 设备状态机
 */
class DeviceStateMachine {
public:
    using StateHandler = std::function<void()>;

    DeviceStateMachine() {
        // 定义状态转移表
        transitions_ = {
            // 从 Uninitialized
            {DeviceState::Uninitialized, DeviceEvent::InitStart,       DeviceState::Initializing},
            // 从 Initializing
            {DeviceState::Initializing,  DeviceEvent::InitSuccess,     DeviceState::Registering},
            {DeviceState::Initializing,  DeviceEvent::InitFailed,      DeviceState::Error},
            // 从 Registering
            {DeviceState::Registering,   DeviceEvent::RegisterSuccess, DeviceState::Running},
            {DeviceState::Registering,   DeviceEvent::RegisterFailed,  DeviceState::Error},
            // 从 Running
            {DeviceState::Running,       DeviceEvent::ConnectionLost,  DeviceState::Recovering},
            {DeviceState::Running,       DeviceEvent::ErrorOccurred,   DeviceState::Error},
            // 从 Error
            {DeviceState::Error,         DeviceEvent::RecoveryStart,   DeviceState::Recovering},
            // 从 Recovering
            {DeviceState::Recovering,    DeviceEvent::RecoverySuccess, DeviceState::Running},
            {DeviceState::Recovering,    DeviceEvent::ErrorOccurred,   DeviceState::Error},
        };
    }

    /**
     * @brief 触发事件，执行状态转移
     */
    void handleEvent(DeviceEvent event) {
        for (const auto& t : transitions_) {
            if (t.from == state_ && t.event == event) {
                LOG_INFO("state_machine", "State transition: %s + %s -> %s",
                        stateName(state_), eventName(event), stateName(t.to));

                state_ = t.to;

                // 执行状态处理器
                auto it = state_handlers_.find(state_);
                if (it != state_handlers_.end()) {
                    it->second();
                }
                return;
            }
        }
        LOG_WARN("state_machine", "No transition for %s + %s",
                stateName(state_), eventName(event));
    }

    /**
     * @brief 获取当前状态
     */
    DeviceState currentState() const { return state_; }

    /**
     * @brief 获取当前状态名称
     */
    std::string currentStateName() const { return stateName(state_); }

    /**
     * @brief 注册状态处理器（进入该状态时调用）
     */
    void onStateChange(DeviceState state, StateHandler handler) {
        state_handlers_[state] = std::move(handler);
    }

    /**
     * @brief 检查是否处于指定状态
     */
    bool isInState(DeviceState state) const { return state_ == state; }

    /**
     * @brief 检查是否处于运行状态
     */
    bool isRunning() const { return state_ == DeviceState::Running; }

private:
    DeviceState state_ = DeviceState::Uninitialized;

    struct Transition {
        DeviceState from;
        DeviceEvent event;
        DeviceState to;
    };

    std::vector<Transition> transitions_;
    std::map<DeviceState, StateHandler> state_handlers_;
};

} // namespace sap
