#include "infra/communica_manage.h"
using namespace std;

/**
 * @brief 构造函数，创建通信设备管理链表的头节点并初始化
 *
 * 分配一个空的 communicateNode 作为链表头节点，com 置为 NULL，id 置为 -1，
 * 后续添加的设备节点将挂载在此头节点之后。
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
 *
 * 作为 std::sort 的自定义比较器，比较二维 vector 中每个元素的第一个值（即设备 ID）。
 *
 * @param a 第一个待比较的 {id, fd} 向量
 * @param b 第二个待比较的 {id, fd} 向量
 * @return true 若 a 的 ID 小于 b 的 ID
 * @return false 否则
 */
bool communicaManage::cmp(const std::vector<int>& a, const std::vector<int>& b)
{
	return a[0] < b[0];
}
/**
 * @brief 按设备 ID 查找通信设备节点
 *
 * 遍历整个链表，返回第一个 id 匹配的节点指针。
 *
 * @param id 待查找的设备 ID
 * @return struct communicateNode* 找到则返回对应节点指针，未找到返回 NULL
 */
struct communicateNode* communicaManage::findcommunicate(int id)
{
	struct communicateNode *temp = communicateNode;
	while (temp!= NULL)
	{
		if (temp->id == id )
		{
			cout << "找到通信设备,ID:"<<temp->id << endl;
			return temp;
		}
		temp = temp->next;
	}
	cout << "未找到你查找的通信设备" << endl;
	return temp;
}

/**
 * @brief 添加带超时阈值的通信设备节点
 *
 * 创建一个包含 fd 和 timeout 的通信节点并挂载到链表末尾。
 * 若链表头节点为空（首次添加），则直接填充头节点；
 * 否则遍历到链表尾部追加新节点。ID 重复时添加失败。
 *
 * @param id 设备 ID（如 Lora=0, WiFi=1, Bluetooth=2, LAN=3）
 * @param fd 设备文件描述符
 * @param timeout 超时阈值，超过此值将触发重新初始化
 * @return true 添加成功
 * @return false 添加失败（ID 已存在）
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
		cout << "添加设备ID:"<<id<<"成功" << endl;
		return true;
	}
	else 
	{
		temp = communicateNode;
		while (temp != NULL)
		{
			if (temp->id == id) {
				cout << "添加节点失败，已经存在ID为：" << id << "的设备" << endl;
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
		cout << "添加设备ID:" << id << "成功" << endl;
		return true;
	}
}

/**
 * @brief 添加空设备节点（未初始化的通信设备）
 *
 * 创建一个 fd 和 timeout 均为默认值（-1）的通信节点，
 * 表示该设备已注册但尚未完成初始化。
 *
 * @param id 设备 ID
 * @return true 添加成功
 * @return false 添加失败（ID 已存在）
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
		cout << "添加设备ID:" << id << "成功" << endl;
		return true;
	}
	else
	{
		temp = communicateNode;
		while (temp != NULL)
		{
			if ((temp->id == id) )
			{
				cout << "添加节点失败，已经存在ID为：" << id << "的设备" << endl;
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
		cout << "添加设备ID:" << id << "成功" << endl;
		return true;
	}
}

/**
 * @brief 添加通信设备节点（常用接口）
 *
 * 创建一个包含 fd 的通信节点，timeout 使用默认值 -1。
 * 这是最常用的添加设备接口，适用于不需要超时管理的场景。
 *
 * @param id 设备 ID
 * @param fd 设备文件描述符
 * @return true 添加成功
 * @return false 添加失败（ID 已存在）
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
		cout << "添加设备ID:" << id << "成功" << endl;
		return true;
	}
	else {
		temp = communicateNode;
		while (temp != NULL)
		{
			if ((temp->id == id) )
			{
				cout << "添加节点失败，已经存在ID为：" << id << "的设备" << endl;
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
		cout << "添加设备ID:" << id << "成功" << endl;
		return true;
	}
}

/**
 * @brief 删除指定 ID 的通信设备节点
 *
 * 使用虚拟头节点（dummy head）技巧简化头节点删除逻辑：
 * 创建一个临时头节点指向当前链表头，遍历查找目标 ID，
 * 找到后摘除节点并释放内存，然后更新链表真实头指针。
 *
 * @param id 待删除的设备 ID
 * @return true 删除成功
 * @return false 未找到对应 ID 的节点
 */
bool communicaManage::deletecommunicateNode(int id)
{
	std::lock_guard<std::recursive_mutex> lock(mtx_);
	/* 使用虚拟头节点技巧：保留 dummy 指针不移动，用 prev 遍历 */
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
			/* 删除后若链表为空，重新创建空头节点避免后续操作崩溃 */
			if (communicateNode == NULL) {
				communicateNode = new struct communicateNode;
				communicateNode->com = NULL;
				communicateNode->enabled = false;
				communicateNode->id = -1;
				communicateNode->next = NULL;
			}
			delete dummy;
			cout << "删除成功" << endl;
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
 *
 * 流程：先通过初始化回调函数获取新的 fd，然后删除旧节点，
 * 再用新 fd 重新添加节点，最后恢复初始化回调函数注册。
 * 若设备未注册初始化回调（reback[0]），则重初始化失败。
 *
 * @param id 待重新初始化的设备 ID
 * @param para 传递给初始化回调函数的参数
 * @return true 重新初始化成功
 * @return false 失败（设备不存在或未注册初始化回调）
 */
bool communicaManage::reinit(int id ,int para)
{
	std::lock_guard<std::recursive_mutex> lock(mtx_);
	struct communicateNode* temp = findcommunicate(id);
	if (temp == NULL)
	{
		cout << "当前没有对应的通信设备,初始化失败" << endl;
		return false;
	}
	if (temp->com->reback[0] == NULL) {
		cout << "重新初始化失败：此通信设备为注册初始化函数，请先添加再重初始化" << endl;
		return false;
	}
	else
	{
		int fd = temp->com->reback[0](para);
		int(*func)(int para) = temp->com->reback[0];
		deletecommunicateNode(id);
		if (!addCommunicateNode(id, fd)) {
			cout << "重新初始化失败：添加节点失败" << endl;
			return false;
		}
		callbackRgist(id,0, func);
		cout << "重新初始化成功" << endl;
		return true;
	}
}                                                         

/**
 * @brief 注册通信设备的回调函数
 *
 * 将指定的函数指针注册到通信设备的 reback 数组中。
 * reback 数组固定3个槽位：
 *   - pos=0: 初始化回调
 *   - pos=1: 发送操作回调
 *   - pos=2: 用户自定义操作回调
 *
 * @param id 设备 ID
 * @param pos 回调槽位索引（0~2）
 * @param function 待注册的函数指针，签名: int func(int)
 * @return true 注册成功
 * @return false 注册失败（pos 超范围或设备不存在）
 */
bool communicaManage::callbackRgist(int id,int pos ,int(*function)(int))
{
	std::lock_guard<std::recursive_mutex> lock(mtx_);
	if (pos < 0 || pos > 2) return false;
	struct communicateNode* temp = findcommunicate(id);
	if (temp==NULL) 
	{
		cout << "当前没有对应的通信设备,回调函数注册失败" << endl;
		return false;
	}
	else 
	{
		temp->com->reback[pos] = function;
		cout << "回调函数注册成功" << endl;
		return true;
	}
}

/**
 * @brief 创建空通信节点（无 fd、无超时）
 *
 * 分配一个 communicate 结构体，fd 置为 -1，所有回调指针置 NULL，
 * timeout 和 timecount 均置为 -1。用于设备仅注册 ID 但尚未初始化的场景。
 *
 * @return communicate* 新创建的通信节点指针
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
 *
 * @param fd 设备文件描述符
 * @return struct communicate* 新创建的通信节点指针
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
 *
 * @param fd 设备文件描述符
 * @param timeout 超时阈值
 * @return struct communicate* 新创建的通信节点指针
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
 *
 * @param id 设备 ID
 * @return int 设备的文件描述符，设备不存在时返回 -1
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
 *
 * 遍历链表，将每个设备的 fd 依次放入 vector 中返回。
 *
 * @return vector<int> 包含所有设备 fd 的列表
 */
vector<int> communicaManage::getAllFd()
{
	std::lock_guard<std::recursive_mutex> lock(mtx_);
	vector<int> result;
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
 *
 * 遍历链表收集每个设备的 {id, fd} 对，然后使用 cmp 函数按 ID 排序。
 *
 * @return vector<vector<int>> 二维向量，每个元素为 {id, fd}，已按 ID 升序排列
 */
vector<vector<int>>  communicaManage::getALLIfd()
{
	std::lock_guard<std::recursive_mutex> lock(mtx_);
	std::vector<vector<int>> result;
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
 *
 * 遍历链表，返回第一个 enabled 为 true 的设备的 fd。
 *
 * @return int 设备 fd，无已使能设备时返回 -1
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
 *
 * @return int 设备 ID，无已使能设备时返回 0
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
 *
 * 将指定 ID 设备的 enabled 标志置为 true，表示该设备已完成注册。
 *
 * @param id 设备 ID
 * @return true 设置成功
 * @return false 设备不存在
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
 *
 * 遍历链表计数所有节点（包括头节点）。
 *
 * @return int 链表中的节点总数
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
 *
 * @param id 待检查的设备 ID
 * @return true 设备存在
 * @return false 设备不存在
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








