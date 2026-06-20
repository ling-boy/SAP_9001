/**
 * @file circuit_breaker.h
 * @brief 熔断器模式实现
 * @details 实现熔断器模式，当失败次数达到阈值时停止尝试，避免无效重试消耗资源
 *
 * 三种状态：
 * - 关闭状态（Closed）：正常通信，统计失败次数
 * - 打开状态（Open）：熔断，停止尝试，等待冷却时间
 * - 半开状态（HalfOpen）：冷却后试探，成功则关闭，失败则重新打开
 *
 * 使用示例：
 *   CircuitBreaker breaker(5, 30000);  // 5次失败后熔断，冷却30秒
 *   if (breaker.allowRequest()) {
 *       // 发送请求
 *       if (success) breaker.recordSuccess();
 *       else breaker.recordFailure();
 *   }
 *
 * @version 1.0
 */
#pragma once

#include <cstdint>
#include <ctime>

namespace sap {

/**
 * @brief 熔断器状态枚举
 */
enum class CircuitState {
    Closed,    /**< 关闭状态：正常通信 */
    Open,      /**< 打开状态：熔断，停止尝试 */
    HalfOpen   /**< 半开状态：试探中 */
};

/**
 * @brief 熔断器
 * @details 当连续失败次数达到阈值时，进入熔断状态，停止尝试
 *          冷却时间过后进入半开状态，尝试一次请求
 *          成功则回到关闭状态，失败则重新进入熔断状态
 */
class CircuitBreaker {
public:
    /**
     * @brief 构造函数
     * @param failure_threshold 触发熔断的失败次数阈值
     * @param cooldown_ms       熔断后的冷却时间（毫秒）
     */
    explicit CircuitBreaker(int failure_threshold = 5, int cooldown_ms = 30000)
        : threshold_(failure_threshold),
          cooldown_ms_(cooldown_ms),
          state_(CircuitState::Closed),
          failure_count_(0),
          last_failure_time_(0) {}

    /**
     * @brief 检查是否允许发送请求
     * @return true 允许，false 熔断中
     */
    bool allowRequest() {
        if (state_ == CircuitState::Closed) {
            return true;
        }

        if (state_ == CircuitState::Open) {
            // 检查冷却时间是否已过
            if (getTickCount() - last_failure_time_ > static_cast<uint64_t>(cooldown_ms_)) {
                state_ = CircuitState::HalfOpen;
                return true;
            }
            return false;
        }

        // HalfOpen 状态：允许试探
        return true;
    }

    /**
     * @brief 记录成功请求
     * @details 重置失败计数器，关闭熔断器
     */
    void recordSuccess() {
        failure_count_ = 0;
        state_ = CircuitState::Closed;
    }

    /**
     * @brief 记录失败请求
     * @details 增加失败计数器，达到阈值时打开熔断器
     */
    void recordFailure() {
        failure_count_++;
        last_failure_time_ = getTickCount();

        if (failure_count_ >= threshold_) {
            state_ = CircuitState::Open;
        }
    }

    /**
     * @brief 获取当前状态
     */
    CircuitState state() const { return state_; }

    /**
     * @brief 获取当前状态的字符串描述
     */
    const char* stateString() const {
        switch (state_) {
            case CircuitState::Closed:   return "Closed";
            case CircuitState::Open:     return "Open";
            case CircuitState::HalfOpen: return "HalfOpen";
            default:                     return "Unknown";
        }
    }

    /**
     * @brief 获取当前失败次数
     */
    int failureCount() const { return failure_count_; }

    /**
     * @brief 获取失败阈值
     */
    int threshold() const { return threshold_; }

    /**
     * @brief 获取冷却时间（毫秒）
     */
    int cooldownMs() const { return cooldown_ms_; }

    /**
     * @brief 重置熔断器到初始状态
     */
    void reset() {
        state_ = CircuitState::Closed;
        failure_count_ = 0;
        last_failure_time_ = 0;
    }

private:
    /**
     * @brief 获取当前时间戳（毫秒）
     * @details 使用 clock_gettime 获取单调时钟
     */
    static uint64_t getTickCount() {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return static_cast<uint64_t>(ts.tv_sec) * 1000 + ts.tv_nsec / 1000000;
    }

    int threshold_;           /**< 触发熔断的失败次数阈值 */
    int cooldown_ms_;         /**< 熔断后的冷却时间（毫秒） */
    CircuitState state_;      /**< 当前状态 */
    int failure_count_;       /**< 当前失败计数 */
    uint64_t last_failure_time_; /**< 最后一次失败的时间戳 */
};

} // namespace sap
