/**
 * @file communicaManage.h
 * @brief 通信设备管理模块接口
 * @details 使用链表管理多种通信设备（LoRa、WiFi、蓝牙、LAN）的初始化、
 *          注册、状态监控和消息收发，支持回调函数注册和设备重初始化
 *
 * 使用智能指针（unique_ptr）管理节点和通信设备内存，消除内存泄漏风险
 */
#pragma once
#ifndef _COMMUNICAMANAGE_H_
#define _COMMUNICAMANAGE_H_

#include <vector>
#include <algorithm>
#include <mutex>
#include <memory>
#include <functional>
#include "infra/circuit_breaker.h"
#include "infra/memory_pool.h"

/** 通信设备最大支持数量 */
#define communicatemax 4

/**
 * @brief 通信设备信息结构体
 * @details 使用 std::function 替代 C 风格函数指针，支持 lambda 和仿函数
 */
struct communicate {
    int fd;                  /**< 通信设备文件描述符 */
    /**
     * @brief 回调函数数组（3个槽位）
     * @details 0: 初始化回调  1: 发送操作回调  2: 用户自定义操作回调
     *          使用 std::function 支持 lambda、仿函数和普通函数
     */
    std::function<int(int)> reback[3];
    int timecount;           /**< 生存计数器 */
    int timeout;             /**< 超时阈值，超过则判定连接中断并触发重初始化 */
    sap::CircuitBreaker breaker;  /**< 熔断器，防止连续失败时无限重试 */
};

/**
 * @brief 通信设备链表节点结构体
 * @details 使用智能指针管理内存，自动释放
 */
struct communicateNode {
    std::unique_ptr<communicate> com;    /**< 通信设备信息（智能指针管理） */
    int id;                              /**< 设备ID（0=LoRa, 1=WiFi, 2=蓝牙, 3=LAN） */
    bool enabled;                        /**< 使能位，标识是否注册成功 */
    std::unique_ptr<communicateNode> next; /**< 指向下一个节点（智能指针管理） */

    communicateNode(int id, std::unique_ptr<communicateNode> next)
        : com(nullptr), id(id), enabled(false), next(std::move(next)) {}
    communicateNode() : com(nullptr), id(-1), enabled(false), next(nullptr) {}
};

/**
 * @brief 通信设备管理类
 * @details 使用智能指针管理链表节点，自动释放内存，消除内存泄漏风险
 */
class communicaManage
{
public:
    /**
     * @brief 添加空设备节点（未初始化的通信设备）
     * @param id 设备ID
     * @return 成功返回true，失败返回false
     */
    bool addCommunicateNode(int id);

    /**
     * @brief 添加通信设备节点（常用接口）
     * @param id 设备ID
     * @param fd 设备文件描述符
     * @return 成功返回true，失败返回false
     */
    bool addCommunicateNode(int id, int fd);

    /**
     * @brief 添加带超时阈值的通信设备节点
     * @param id      设备ID
     * @param fd      设备文件描述符
     * @param timeout 超时阈值
     * @return 成功返回true，失败返回false
     */
    bool addCommunicateNode(int id, int fd, int timeout);

    /**
     * @brief 删除指定ID的通信设备节点
     * @param id 设备ID
     * @return 成功返回true，失败返回false
     */
    bool deletecommunicateNode(int id);

    /**
     * @brief 重新初始化指定通信设备
     * @param id   设备ID
     * @param para 回调函数参数
     * @return 成功返回true，失败返回false
     */
    bool reinit(int id, int para);

    /**
     * @brief 注册通信设备的回调函数
     * @param id       设备ID
     * @param pos      回调槽位（0=初始化, 1=发送, 2=自定义）
     * @param function 回调函数（支持 lambda、仿函数、普通函数）
     * @return 成功返回true，失败返回false
     */
    bool callbackRgist(int id, int pos, std::function<int(int)> function);

    /**
     * @brief 获取单个设备的文件描述符
     * @param id 设备ID
     * @return 成功返回fd，失败返回-1
     */
    int getSingleFd(int id);

    /**
     * @brief 获取所有设备的文件描述符列表
     * @return fd列表
     */
    std::vector<int> getAllFd();

    /**
     * @brief 获取所有设备的ID和FD，按ID从小到大排序返回
     * @return 二维数组，每行包含{id, fd}
     */
    std::vector<std::vector<int>> getALLIfd();

    /**
     * @brief 获取第一个已注册成功（enabled）设备的文件描述符
     * @return 成功返回fd，无可用设备返回-1
     */
    int getSuccessFd();

    /**
     * @brief 获取第一个已注册成功（enabled）设备的ID
     * @return 成功返回设备ID，无可用设备返回-1
     */
    int getSuccessId();

    /**
     * @brief 设置设备为注册成功使能状态
     * @param id 设备ID
     * @return 成功返回true，失败返回false
     */
    bool setEnable(int id);

    /**
     * @brief 获取设备链表大小
     * @return 设备数量
     */
    int getSize();

    /**
     * @brief 检查指定ID的设备是否存在
     * @param id 设备ID
     * @return 存在返回true，不存在返回false
     */
    bool isExist(int id);

    /** @brief 构造函数，创建头节点并初始化 */
    communicaManage();

    /** @brief 析构函数，智能指针自动释放内存 */
    ~communicaManage() = default;

    /** @brief 禁止拷贝构造（链表不可浅拷贝） */
    communicaManage(const communicaManage&) = delete;

    /** @brief 禁止拷贝赋值（链表不可浅拷贝） */
    communicaManage& operator=(const communicaManage&) = delete;

private:
    std::unique_ptr<communicateNode> head_; /**< 链表头节点（智能指针管理） */

    /**
     * @brief 按设备ID查找通信设备节点
     * @param id 设备ID
     * @return 找到返回节点指针，未找到返回nullptr
     */
    communicateNode* findcommunicate(int id);

    /** @brief 创建空通信节点（无fd、无超时） */
    std::unique_ptr<communicate> createComnode();

    /**
     * @brief 创建带fd的通信节点（无超时）
     * @param fd 设备文件描述符
     */
    std::unique_ptr<communicate> createComnode(int fd);

    /**
     * @brief 创建带fd和超时阈值的通信节点
     * @param fd      设备文件描述符
     * @param timeout 超时阈值
     */
    std::unique_ptr<communicate> createComnode(int fd, int timeout);

    /** @brief 排序比较函数，用于按ID从小到大排序 */
    static bool cmp(const std::vector<int>& a, const std::vector<int>& b);

    /** @brief 互斥锁，保护链表操作的线程安全 */
    std::recursive_mutex mtx_;

    /** @brief communicate 结构体内存池（最多4个设备） */
    sap::MemoryPool<communicate, 4> comm_pool_;
};
#endif
