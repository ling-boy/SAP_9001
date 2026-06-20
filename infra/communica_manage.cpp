/**
 * @file communica_manage.cpp
 * @brief 通信设备管理模块实现
 * @details 使用智能指针管理链表节点，自动释放内存，消除内存泄漏风险
 */

#include "infra/communica_manage.h"
#include "infra/logger.h"

/**
 * @brief 构造函数，创建通信设备管理链表的头节点并初始化
 */
communicaManage::communicaManage()
    : head_(std::make_unique<communicateNode>())
{
}

/**
 * @brief 排序比较函数，用于按ID从小到大排序
 */
bool communicaManage::cmp(const std::vector<int>& a, const std::vector<int>& b)
{
    return a[0] < b[0];
}

/**
 * @brief 按设备 ID 查找通信设备节点
 */
communicateNode* communicaManage::findcommunicate(int id)
{
    communicateNode* temp = head_.get();
    while (temp != nullptr)
    {
        if (temp->id == id)
        {
            LOG_DEBUG("comm_mgr", "Found device ID=%d", temp->id);
            return temp;
        }
        temp = temp->next.get();
    }
    LOG_DEBUG("comm_mgr", "Device ID=%d not found", id);
    return nullptr;
}

/**
 * @brief 添加带超时阈值的通信设备节点
 */
bool communicaManage::addCommunicateNode(int id, int fd, int timeout)
{
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    auto com = createComnode(fd, timeout);

    if (head_->com == nullptr)
    {
        head_->com = std::move(com);
        head_->id = id;
        head_->enabled = false;
        LOG_INFO("comm_mgr", "Add device ID=%d success", id);
        return true;
    }
    else
    {
        communicateNode* temp = head_.get();
        while (temp != nullptr)
        {
            if (temp->id == id) {
                LOG_WARN("comm_mgr", "Add node failed, ID=%d already exists", id);
                return false;
            }
            if (temp->next == nullptr)
                break;
            temp = temp->next.get();
        }
        auto newNode = std::make_unique<communicateNode>();
        newNode->com = std::move(com);
        newNode->id = id;
        newNode->enabled = false;
        temp->next = std::move(newNode);
        LOG_INFO("comm_mgr", "Add device ID=%d success", id);
        return true;
    }
}

/**
 * @brief 添加空设备节点（未初始化的通信设备）
 */
bool communicaManage::addCommunicateNode(int id)
{
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    auto com = createComnode();

    if (head_->com == nullptr)
    {
        head_->com = std::move(com);
        head_->id = id;
        head_->enabled = false;
        LOG_INFO("comm_mgr", "Add device ID=%d success", id);
        return true;
    }
    else
    {
        communicateNode* temp = head_.get();
        while (temp != nullptr)
        {
            if (temp->id == id)
            {
                LOG_WARN("comm_mgr", "Add node failed, ID=%d already exists", id);
                return false;
            }
            if (temp->next == nullptr)
                break;
            temp = temp->next.get();
        }
        auto newNode = std::make_unique<communicateNode>();
        newNode->com = std::move(com);
        newNode->id = id;
        newNode->enabled = false;
        temp->next = std::move(newNode);
        LOG_INFO("comm_mgr", "Add device ID=%d success", id);
        return true;
    }
}

/**
 * @brief 添加通信设备节点（常用接口）
 */
bool communicaManage::addCommunicateNode(int id, int fd)
{
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    auto com = createComnode(fd);

    if (head_->com == nullptr)
    {
        head_->com = std::move(com);
        head_->id = id;
        head_->enabled = false;
        LOG_INFO("comm_mgr", "Add device ID=%d success", id);
        return true;
    }
    else
    {
        communicateNode* temp = head_.get();
        while (temp != nullptr)
        {
            if (temp->id == id)
            {
                LOG_WARN("comm_mgr", "Add node failed, ID=%d already exists", id);
                return false;
            }
            if (temp->next == nullptr)
                break;
            temp = temp->next.get();
        }
        auto newNode = std::make_unique<communicateNode>();
        newNode->com = std::move(com);
        newNode->id = id;
        newNode->enabled = false;
        temp->next = std::move(newNode);
        LOG_INFO("comm_mgr", "Add device ID=%d success", id);
        return true;
    }
}

/**
 * @brief 删除指定 ID 的通信设备节点
 */
bool communicaManage::deletecommunicateNode(int id)
{
    std::lock_guard<std::recursive_mutex> lock(mtx_);

    // 特殊情况：删除头节点
    if (head_->id == id)
    {
        if (head_->next)
        {
            head_ = std::move(head_->next);
        }
        else
        {
            head_ = std::make_unique<communicateNode>();
        }
        LOG_INFO("comm_mgr", "Delete device ID=%d success", id);
        return true;
    }

    // 查找要删除的节点
    communicateNode* prev = head_.get();
    while (prev->next != nullptr)
    {
        if (prev->next->id == id)
        {
            auto toDelete = std::move(prev->next);
            prev->next = std::move(toDelete->next);
            LOG_INFO("comm_mgr", "Delete device ID=%d success", id);
            return true;
        }
        prev = prev->next.get();
    }
    return false;
}

/**
 * @brief 重新初始化指定通信设备
 * @details 使用熔断器保护，防止连续失败时无限重试
 */
bool communicaManage::reinit(int id, int para)
{
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    communicateNode* temp = findcommunicate(id);
    if (temp == nullptr)
    {
        LOG_ERROR("comm_mgr", "%s", "No device found, reinit failed");
        return false;
    }

    // 熔断检查：如果熔断器打开，跳过重初始化
    if (!temp->com->breaker.allowRequest()) {
        LOG_WARN("comm_mgr", "Device ID=%d circuit breaker OPEN, skip reinit (failures=%d)",
                 id, temp->com->breaker.failureCount());
        return false;
    }

    if (temp->com->reback[0] == nullptr) {
        LOG_ERROR("comm_mgr", "%s", "Reinit failed: no init callback registered");
        return false;
    }
    else
    {
        int fd = temp->com->reback[0](para);

        // 重初始化失败，记录到熔断器
        if (fd < 0) {
            temp->com->breaker.recordFailure();
            LOG_ERROR("comm_mgr", "Reinit device ID=%d failed, breaker failures=%d/%d",
                      id, temp->com->breaker.failureCount(), temp->com->breaker.threshold());
            return false;
        }

        // 重初始化成功，重置熔断器
        temp->com->breaker.recordSuccess();
        auto func = temp->com->reback[0];  // 保存回调函数
        deletecommunicateNode(id);
        if (!addCommunicateNode(id, fd)) {
            LOG_ERROR("comm_mgr", "%s", "Reinit failed: add node failed");
            return false;
        }
        callbackRgist(id, 0, std::move(func));
        LOG_INFO("comm_mgr", "Reinit device ID=%d success", id);
        return true;
    }
}

/**
 * @brief 注册通信设备的回调函数
 */
bool communicaManage::callbackRgist(int id, int pos, std::function<int(int)> function)
{
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    if (pos < 0 || pos > 2) return false;
    communicateNode* temp = findcommunicate(id);
    if (temp == nullptr)
    {
        LOG_ERROR("comm_mgr", "%s", "No device found, callback register failed");
        return false;
    }
    else
    {
        temp->com->reback[pos] = std::move(function);
        LOG_INFO("comm_mgr", "Callback registered for device ID=%d pos=%d", id, pos);
        return true;
    }
}

/**
 * @brief 创建空通信节点（无 fd、无超时）
 */
std::unique_ptr<communicate> communicaManage::createComnode()
{
    auto com = std::make_unique<communicate>();
    com->fd = -1;
    for (int i = 0; i < 3; i++)
    {
        com->reback[i] = nullptr;
    }
    com->timecount = -1;
    com->timeout = -1;
    return com;
}

/**
 * @brief 创建带 fd 的通信节点（无超时）
 */
std::unique_ptr<communicate> communicaManage::createComnode(int fd)
{
    auto com = std::make_unique<communicate>();
    com->fd = fd;
    for (int i = 0; i < 3; i++)
    {
        com->reback[i] = nullptr;
    }
    com->timecount = -1;
    com->timeout = -1;
    return com;
}

/**
 * @brief 创建带 fd 和超时阈值的通信节点
 */
std::unique_ptr<communicate> communicaManage::createComnode(int fd, int timeout)
{
    auto com = std::make_unique<communicate>();
    com->fd = fd;
    for (int i = 0; i < 3; i++)
    {
        com->reback[i] = nullptr;
    }
    com->timecount = -1;
    com->timeout = timeout;
    return com;
}

/**
 * @brief 获取单个设备的文件描述符
 */
int communicaManage::getSingleFd(int id)
{
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    communicateNode* com = findcommunicate(id);
    if (com == nullptr) return -1;
    return com->com->fd;
}

/**
 * @brief 获取所有设备的文件描述符列表
 */
std::vector<int> communicaManage::getAllFd()
{
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    std::vector<int> result;
    communicateNode* temp = head_.get();
    while (temp != nullptr)
    {
        if (temp->com != nullptr)
            result.push_back(temp->com->fd);
        temp = temp->next.get();
    }
    return result;
}

/**
 * @brief 获取所有设备的 ID 和 FD，按 ID 从小到大排序返回
 */
std::vector<std::vector<int>> communicaManage::getALLIfd()
{
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    std::vector<std::vector<int>> result;
    std::vector<int> vec;
    communicateNode* temp = head_.get();
    while (temp != nullptr)
    {
        if (temp->com != nullptr) {
            vec.push_back(temp->id);
            vec.push_back(temp->com->fd);
            result.push_back(vec);
            vec.clear();
        }
        temp = temp->next.get();
    }
    sort(result.begin(), result.end(), cmp);
    return result;
}

/**
 * @brief 获取第一个已注册成功（enabled）设备的文件描述符
 */
int communicaManage::getSuccessFd()
{
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    communicateNode* temp = head_.get();
    while (temp != nullptr)
    {
        if (temp->enabled == true && temp->com != nullptr)
        {
            return temp->com->fd;
        }
        temp = temp->next.get();
    }
    return -1;
}

/**
 * @brief 获取第一个已注册成功（enabled）设备的 ID
 */
int communicaManage::getSuccessId()
{
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    communicateNode* temp = head_.get();
    while (temp != nullptr)
    {
        if (temp->enabled == true)
        {
            return temp->id;
        }
        temp = temp->next.get();
    }
    return -1;
}

/**
 * @brief 设置设备为注册成功使能状态
 */
bool communicaManage::setEnable(int id)
{
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    communicateNode* temp = findcommunicate(id);
    if (temp != nullptr)
    {
        temp->enabled = true;
        return true;
    }
    return false;
}

/**
 * @brief 获取设备链表大小
 */
int communicaManage::getSize()
{
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    int size = 0;
    communicateNode* temp = head_.get();
    while (temp != nullptr)
    {
        size++;
        temp = temp->next.get();
    }
    return size;
}

/**
 * @brief 检查指定 ID 的设备是否存在
 */
bool communicaManage::isExist(int id)
{
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    communicateNode* temp = head_.get();
    while (temp != nullptr) {
        if (temp->id == id) return true;
        temp = temp->next.get();
    }
    return false;
}
