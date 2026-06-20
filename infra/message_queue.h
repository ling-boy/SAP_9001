/**
 * @file message_queue.h
 * @brief 线程安全的消息队列模板类
 * @details 封装 std::queue + std::mutex + std::condition_variable，
 *          提供多线程安全的阻塞/非阻塞 push/pop 操作
 */
#pragma once
#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_

#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cstddef>

/**
 * @brief 线程安全的消息队列
 * @tparam T 队列元素类型
 */
template<typename T>
class MessageQueue {
public:
    MessageQueue() = default;
    ~MessageQueue() = default;

    /* 禁止拷贝和赋值 */
    MessageQueue(const MessageQueue&) = delete;
    MessageQueue& operator=(const MessageQueue&) = delete;

    /**
     * @brief 向队列推送一个元素，唤醒一个等待线程
     * @param item 待推送的元素
     */
    void push(T item) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(item));
        }
        cv_.notify_one();
    }

    /**
     * @brief 从队列弹出一个元素（阻塞等待）
     * @param item 输出参数，存储弹出的元素
     * @param timeout_ms 超时时间（毫秒），-1 表示无限等待
     * @return true 成功弹出，false 超时或队列已关闭
     */
    bool pop(T& item, int timeout_ms = -1) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (timeout_ms < 0) {
            cv_.wait(lock, [this] { return !queue_.empty() || closed_; });
        } else {
            if (!cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                              [this] { return !queue_.empty() || closed_; })) {
                return false;
            }
        }
        if (closed_ && queue_.empty()) {
            return false;
        }
        item = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    /**
     * @brief 非阻塞尝试弹出一个元素
     * @param item 输出参数，存储弹出的元素
     * @return true 成功弹出，false 队列为空
     */
    bool tryPop(T& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (closed_ && queue_.empty()) {
            return false;
        }
        if (queue_.empty()) {
            return false;
        }
        item = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    /**
     * @brief 获取队列当前大小
     */
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    /**
     * @brief 清空队列
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::queue<T> empty;
        queue_.swap(empty);
    }

    /**
     * @brief 关闭队列，唤醒所有等待线程
     */
    void close() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            closed_ = true;
        }
        cv_.notify_all();
    }

    /**
     * @brief 弹出所有元素到输出向量（用于持久化等批量操作）
     * @param items 输出向量，存储所有弹出的元素
     * @return 弹出的元素数量
     */
    size_t popAll(std::vector<T>& items) {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!queue_.empty()) {
            items.push_back(std::move(queue_.front()));
            queue_.pop();
        }
        return items.size();
    }

    /**
     * @brief 无锁弹出所有元素（用于信号处理器等单线程场景，避免死锁）
     * @warning 非线程安全，调用者需确保无其他线程并发访问
     * @note std::string move 和 vector::push_back 不是 async-signal-safe，
     *       严格来说在信号处理器中使用仍有 UB 风险，但 Linux 实践中通常可行
     */
    size_t drainUnsafe(std::vector<T>& items) {
        while (!queue_.empty()) {
            items.push_back(std::move(queue_.front()));
            queue_.pop();
        }
        return items.size();
    }

    /**
     * @brief 检查队列是否为空
     */
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    bool closed_ = false;
};

#endif
