/**
 * @file memory_pool.h
 * @brief 固定大小内存池（模板类）
 * @details 预分配固定数量的内存块，提供 O(1) 的分配和释放操作
 *          解决嵌入式系统长时间运行的内存碎片问题
 *
 * 使用示例：
 *   MemoryPool<communicateNode, 4> pool;
 *   auto* node = pool.allocate();
 *   pool.deallocate(node);
 *
 * @version 1.0
 */
#pragma once

#include <cstddef>
#include <new>
#include <stack>
#include <type_traits>
#include "infra/logger.h"

namespace sap {

/**
 * @brief 固定大小内存池
 * @tparam T        对象类型
 * @tparam PoolSize 池中预分配的块数量
 *
 * @warning **仅限单线程使用**：本类的 allocate()/deallocate() 等方法均无锁保护，
 *          多线程并发调用会导致数据竞争和未定义行为。
 *          如需多线程访问，调用方应自行在外层加锁保护。
 */
template<typename T, size_t PoolSize = 16>
class MemoryPool {
public:
    /**
     * @brief 构造函数，预分配所有内存块
     */
    MemoryPool() : allocated_count_(0) {
        // 初始化空闲列表
        for (size_t i = 0; i < PoolSize; ++i) {
            free_list_.push(&storage_[i]);
        }
        LOG_INFO("memory_pool", "Created pool for %s, size=%zu",
                 typeid(T).name(), PoolSize);
    }

    /**
     * @brief 析构函数
     * @note 调用者必须确保所有分配的对象已被释放
     */
    ~MemoryPool() {
        if (allocated_count_ > 0) {
            LOG_WARN("memory_pool", "Pool destroyed with %zu objects still allocated",
                     allocated_count_);
        }
    }

    // 禁止拷贝
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    /**
     * @brief 分配内存块并构造对象
     * @tparam Args 构造函数参数类型
     * @param args  构造函数参数
     * @return 成功返回对象指针，池已满返回 nullptr
     */
    template<typename... Args>
    T* allocate(Args&&... args) {
        if (free_list_.empty()) {
            LOG_ERROR("memory_pool", "Pool exhausted for %s", typeid(T).name());
            return nullptr;
        }

        // 从空闲列表获取一块内存
        void* block = free_list_.top();
        free_list_.pop();

        // 在该内存上构造对象（placement new）
        T* obj = new (block) T(std::forward<Args>(args)...);
        allocated_count_++;

        LOG_DEBUG("memory_pool", "Allocated %s, used=%zu/%zu",
                  typeid(T).name(), allocated_count_, PoolSize);
        return obj;
    }

    /**
     * @brief 释放内存块（析构对象并归还到空闲列表）
     * @param ptr 要释放的对象指针
     */
    void deallocate(T* ptr) {
        if (ptr == nullptr) {
            LOG_WARN("memory_pool", "Attempt to deallocate nullptr");
            return;
        }

        // 检查指针是否属于本池
        if (!belongsToPool(ptr)) {
            LOG_ERROR("memory_pool", "Pointer does not belong to this pool");
            return;
        }

        // 析构对象
        ptr->~T();

        // 归还到空闲列表
        free_list_.push(ptr);
        allocated_count_--;

        LOG_DEBUG("memory_pool", "Deallocated %s, used=%zu/%zu",
                  typeid(T).name(), allocated_count_, PoolSize);
    }

    /**
     * @brief 获取已分配的对象数量
     */
    size_t allocatedCount() const { return allocated_count_; }

    /**
     * @brief 获取池的总容量
     */
    size_t capacity() const { return PoolSize; }

    /**
     * @brief 获取剩余可用块数量
     */
    size_t available() const { return free_list_.size(); }

    /**
     * @brief 检查池是否已满
     */
    bool isFull() const { return free_list_.empty(); }

    /**
     * @brief 检查池是否为空（无已分配对象）
     */
    bool isEmpty() const { return allocated_count_ == 0; }

private:
    /**
     * @brief 检查指针是否属于本池的内存范围
     */
    bool belongsToPool(const T* ptr) const {
        const void* p = static_cast<const void*>(ptr);
        const void* start = static_cast<const void*>(&storage_[0]);
        const void* end = static_cast<const void*>(&storage_[PoolSize]);
        return p >= start && p < end;
    }

    /**
     * @brief 对齐的存储空间
     * @details 使用 aligned_storage 确保内存对齐
     */
    typename std::aligned_storage<sizeof(T), alignof(T)>::type storage_[PoolSize];

    /**
     * @brief 空闲列表（使用栈实现）
     */
    std::stack<void*> free_list_;

    /**
     * @brief 已分配对象计数
     */
    size_t allocated_count_;
};

} // namespace sap
