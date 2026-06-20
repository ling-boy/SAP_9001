/**
 * @file retry_policy.h
 * @brief 指数退避重试策略
 * @details 实现指数退避算法，避免在网络不稳定时频繁重试
 *
 * 使用示例：
 *   ExponentialBackoff backoff(1000, 60000);  // 基础1秒，最大60秒
 *   int delay = backoff.nextDelay();  // 1秒
 *   delay = backoff.nextDelay();      // 2秒
 *   delay = backoff.nextDelay();      // 4秒
 *   backoff.reset();                  // 重置计数器
 *
 * @version 1.0
 */
#pragma once

#include <algorithm>
#include <cstdint>

namespace sap {

/**
 * @brief 指数退避重试策略
 * @details 每次重试的等待时间按指数增长，直到达到最大延迟
 *          公式：delay = min(base_delay × 2^retry_count, max_delay)
 */
class ExponentialBackoff {
public:
    /**
     * @brief 构造函数
     * @param base_ms  基础延迟（毫秒）
     * @param max_ms   最大延迟（毫秒）
     */
    explicit ExponentialBackoff(int base_ms = 1000, int max_ms = 60000)
        : base_ms_(base_ms), max_ms_(max_ms), retry_count_(0) {}

    /**
     * @brief 获取下一次重试的延迟时间
     * @return 延迟时间（毫秒）
     */
    int nextDelay() {
        // 计算指数退避：base * 2^count
        int delay = base_ms_ * (1 << retry_count_);
        // 限制最大延迟
        delay = std::min(delay, max_ms_);
        // 增加重试计数
        retry_count_++;
        return delay;
    }

    /**
     * @brief 重置重试计数器
     * @details 在成功操作后调用，重置退避状态
     */
    void reset() { retry_count_ = 0; }

    /**
     * @brief 获取当前重试次数
     */
    int retryCount() const { return retry_count_; }

    /**
     * @brief 获取基础延迟
     */
    int baseDelay() const { return base_ms_; }

    /**
     * @brief 获取最大延迟
     */
    int maxDelay() const { return max_ms_; }

private:
    int base_ms_;      /**< 基础延迟（毫秒） */
    int max_ms_;       /**< 最大延迟（毫秒） */
    int retry_count_;  /**< 重试计数器 */
};

} // namespace sap
