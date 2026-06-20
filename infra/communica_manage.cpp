#include "infra/communica_manage.h"
#include "infra/logger.h"

/**
 * @brief 构造函数，创建通信设备管理链表的头节点并初始化
 */
communicaManage::communicaManage()
{
	communicateNode = new struct communicateNode;
	communicateNode->com = NULL;
	communicateNode->enabled = false;
	communicateNode->id = -1;
	communicateNode->next = NULL;
}

/**
 * @brief 析构函数，遍历释放链表所有节点内存
 */
communicaManage::~communicaManage()
{
	struct communicateNode* current = communicateNode;
	while (current != NULL) {
		struct communicateNode* next = current->next;
		if (current->com != NULL) {
			delete current->com;
		}
		delete current;
		current = next;
	}
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
struct communicateNode* communicaManage::findcommunicate(int id)
{
	struct communicateNode *temp = communicateNode;
	while (temp!= NULL)
	{
		if (temp->id == id )
		{
			LOG_DEBUG("comm_mgr", "Found device ID=%d", temp->id);
			return temp;
		}
		temp = temp->next;
	}
	LOG_DEBUG("comm_mgr", "Device ID=%d not found", id);
	return temp;
}

/**
 * @brief 添加带超时阈值的通信设备节点
 */
bool communicaManage::addCommunicateNode(int id, int fd, int timeout)
{
	std::lock_guard<std::recursive_mutex> lock(mtx_);
	struct communicateNode *temp=NULL;
	struct communicate * com = createComnode(fd,timeout);
	if (communicateNode->com == NULL)
	{
		communicateNode->com = com;
		communicateNode->id = id;
		communicateNode->enabled = false;
		communicateNode->next = NULL;
		LOG_INFO("comm_mgr", "Add device ID=%d success", id);
		return true;
	}
	else
	{
		temp = communicateNode;
		while (temp != NULL)
		{
			if (temp->id == id) {
				LOG_WARN("comm_mgr", "Add node failed, ID=%d already exists", id);
				delete com;
				return false;
			}
			if (temp->next == NULL)
				break;
			temp = temp->next;
		}
		struct communicateNode *temp1 = new struct communicateNode;
		temp1->com = com;
		temp1->id = id;
		temp1->enabled = false;
		temp1->next = NULL;
		temp->next = temp1;
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
	struct communicateNode *temp = NULL;
	struct communicate * com = createComnode();
	if (communicateNode->com == NULL)
	{
		communicateNode->com = com;
		communicateNode->id = id;
		communicateNode->enabled = false;
		communicateNode->next = NULL;
		LOG_INFO("comm_mgr", "Add device ID=%d success", id);
		return true;
	}
	else
	{
		temp = communicateNode;
		while (temp != NULL)
		{
			if ((temp->id == id) )
			{
				LOG_WARN("comm_mgr", "Add node failed, ID=%d already exists", id);
				delete com;
				return false;
			}
			if (temp->next == NULL)
				break;
			temp = temp->next;

		}
		struct communicateNode *temp1 = new struct communicateNode;
		temp1->com = com;
		temp1->id = id;
		temp1->enabled = false;
		temp1->next = NULL;
		temp->next = temp1;
		LOG_INFO("comm_mgr", "Add device ID=%d success", id);
		return true;
	}
}

/**
 * @brief 添加通信设备节点（常用接口）
 */
bool communicaManage::addCommunicateNode(int id,int fd)
{
	std::lock_guard<std::recursive_mutex> lock(mtx_);
	struct communicateNode *temp = NULL;
	struct communicate * com = createComnode(fd);
	if (communicateNode->com == NULL)
	{
		communicateNode->com = com;
		communicateNode->id = id;
		communicateNode->enabled = false;
		communicateNode->next = NULL;
		LOG_INFO("comm_mgr", "Add device ID=%d success", id);
		return true;
	}
	else {
		temp = communicateNode;
		while (temp != NULL)
		{
			if ((temp->id == id) )
			{
				LOG_WARN("comm_mgr", "Add node failed, ID=%d already exists", id);
				delete com;
				return false;
			}
			if (temp->next == NULL)
				break;
			temp = temp->next;
		}
		struct communicateNode *temp1 = new struct communicateNode;
		temp1->com = com;
		temp1->id = id;
		temp1->enabled = false;
		temp1->next = NULL;
		temp->next = temp1;
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
	/* 使用虚拟头节点技巧 */
	struct communicateNode *dummy = new struct communicateNode(-1, communicateNode);
	struct communicateNode *prev = dummy;
	while (prev->next != NULL)
	{
		if (prev->next->id == id)
		{
			struct communicateNode *temp = prev->next;
			prev->next = temp->next;
			if (temp->com != NULL) delete temp->com;
			delete temp;
			communicateNode = dummy->next;
			/* 删除后若链表为空，重新创建空头节点 */
			if (communicateNode == NULL) {
				communicateNode = new struct communicateNode;
				communicateNode->com = NULL;
				communicateNode->enabled = false;
				communicateNode->id = -1;
				communicateNode->next = NULL;
			}
			delete dummy;
			LOG_INFO("comm_mgr", "Delete device ID=%d success", id);
			return true;
		}
		prev = prev->next;
	}
	communicateNode = dummy->next;
	delete dummy;
	return false;
}

/**
 * @brief 重新初始化指定通信设备
 */
bool communicaManage::reinit(int id ,int para)
{
	std::lock_guard<std::recursive_mutex> lock(mtx_);
	struct communicateNode* temp = findcommunicate(id);
	if (temp == NULL)
	{
		LOG_ERROR("comm_mgr", "No device found, reinit failed");
		return false;
	}
	if (temp->com->reback[0] == NULL) {
		LOG_ERROR("comm_mgr", "Reinit failed: no init callback registered");
		return false;
	}
	else
	{
		int fd = temp->com->reback[0](para);
		int(*func)(int para) = temp->com->reback[0];
		deletecommunicateNode(id);
		if (!addCommunicateNode(id, fd)) {
			LOG_ERROR("comm_mgr", "Reinit failed: add node failed");
			return false;
		}
		callbackRgist(id,0, func);
		LOG_INFO("comm_mgr", "Reinit device ID=%d success", id);
		return true;
	}
}

/**
 * @brief 注册通信设备的回调函数
 */
bool communicaManage::callbackRgist(int id,int pos ,int(*function)(int))
{
	std::lock_guard<std::recursive_mutex> lock(mtx_);
	if (pos < 0 || pos > 2) return false;
	struct communicateNode* temp = findcommunicate(id);
	if (temp==NULL)
	{
		LOG_ERROR("comm_mgr", "No device found, callback register failed");
		return false;
	}
	else
	{
		temp->com->reback[pos] = function;
		LOG_INFO("comm_mgr", "Callback registered for device ID=%d pos=%d", id, pos);
		return true;
	}
}

/**
 * @brief 创建空通信节点（无 fd、无超时）
 */
communicate * communicaManage::createComnode()
{
	communicate * com = new communicate;
	com->fd = -1;
	for (int i = 0; i < 3; i++)
	{
		com->reback[i] = NULL;
	}
	com->timecount = -1;
	com->timeout = -1;
	return com;
}

/**
 * @brief 创建带 fd 的通信节点（无超时）
 */
struct communicate* communicaManage::createComnode(int fd)
{
	communicate * com = new communicate;
	com->fd = fd;
	for (int i = 0; i < 3;i++)
	{
		com->reback[i] = NULL;
	}
	com->timecount = -1;
	com->timeout = -1;
	return com;
}

/**
 * @brief 创建带 fd 和超时阈值的通信节点
 */
struct communicate* communicaManage::createComnode(int fd,int timeout)
{
	communicate * com = new communicate;
	com->fd = fd;
	for (int i = 0; i < 3; i++)
	{
		com->reback[i] = NULL;
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
	struct communicateNode* com=findcommunicate(id);
	if (com == NULL)  return -1;
	return com->com->fd;
}

/**
 * @brief 获取所有设备的文件描述符列表
 */
std::vector<int> communicaManage::getAllFd()
{
	std::lock_guard<std::recursive_mutex> lock(mtx_);
	std::vector<int> result;
	struct communicateNode * temp = communicateNode;
	while (temp != NULL)
	{
		if (temp->com != NULL)
			result.push_back(temp->com->fd);
		temp = temp->next;
	}
	return result;
}

/**
 * @brief 获取所有设备的 ID 和 FD，按 ID 从小到大排序返回
 */
std::vector<std::vector<int>>  communicaManage::getALLIfd()
{
	std::lock_guard<std::recursive_mutex> lock(mtx_);
	std::vector<std::vector<int>> result;
	std::vector<int> vec;
	struct communicateNode * temp = communicateNode;
	while (temp != NULL)
	{
		if (temp->com != NULL) {
			vec.push_back(temp->id);
			vec.push_back(temp->com->fd);
			result.push_back(vec);
			vec.clear();
		}
		temp = temp->next;
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
	struct communicateNode* temp = communicateNode;
	while (temp!=NULL)
	{
		if (temp->enabled == true && temp->com != NULL)
		{
			return temp->com->fd;
		}
		temp = temp->next;
	}
	return -1;
}

/**
 * @brief 获取第一个已注册成功（enabled）设备的 ID
 */
int communicaManage::getSuccessId()
{
	std::lock_guard<std::recursive_mutex> lock(mtx_);
	struct communicateNode* temp = communicateNode;
	while (temp!=NULL)
	{
		if (temp->enabled == true)
		{
			return temp->id;
		}
		temp = temp->next;
	}
	return -1;
}

/**
 * @brief 设置设备为注册成功使能状态
 */
bool communicaManage::setEnable(int id)
{
	std::lock_guard<std::recursive_mutex> lock(mtx_);
	struct communicateNode* temp = NULL;
	if ((temp=findcommunicate(id)) != NULL)
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
	int size=0;
	struct communicateNode * temp= communicateNode;
	while (temp != NULL)
	{
		size++;
		temp = temp->next;
	}
	return size;
}

/**
 * @brief 检查指定 ID 的设备是否存在
 */
bool communicaManage::isExist(int id)
{
	std::lock_guard<std::recursive_mutex> lock(mtx_);
	struct communicateNode * temp = communicateNode;
	while (temp != NULL) {
		if (temp->id == id) return true;
		temp = temp->next;
	}
	return false;
}
