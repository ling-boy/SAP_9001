#include "infra/software_wdt.h"
#include "infra/logger.h"

/**
 * @brief 构造函数，初始化看门狗数组
 */
CSoftwareWdt::CSoftwareWdt()
{
	int bId;
	for (bId = 0; bId < REQUEST_WDT_MAX_NUM; bId++)
		m_fReqWdtFlg[bId] = false;
	memset(m_wWdtTimeout,0, sizeof(m_wWdtTimeout));
	memset(m_wWdtCount,0, sizeof(m_wWdtCount));
}

CSoftwareWdt::~CSoftwareWdt()
{
}

/**
 * @brief 申请软件看门狗ID
 */
int CSoftwareWdt::RequestSoftwareWdtID(const char *cThreadName, int wTimeout )
{
	std::lock_guard<std::recursive_mutex> lock(mtx_);
	int bId;
	int iRet = -1;
	for (bId = 0; bId < REQUEST_WDT_MAX_NUM; bId++)
	{
		if (m_fReqWdtFlg[bId] == false)
		{
			m_wWdtTimeout[bId] = wTimeout;
			m_wWdtCount[bId] = 0;
			m_fReqWdtFlg[bId] = true;
			m_wwdtName[bId]=cThreadName;
			LOG_INFO("wdt", "Thread %s request ID=%d, timeout=%d", cThreadName, bId, wTimeout);
			return bId;
		}
	}

	return iRet;
}

/**
 * @brief 释放指定线程的看门狗ID
 */
bool CSoftwareWdt::ReleaseSoftwareWdtID(const char *cThreadName, int wWdtId)
{
	std::lock_guard<std::recursive_mutex> lock(mtx_);
	bool fRet = false;

	if (wWdtId < 0 || wWdtId >= REQUEST_WDT_MAX_NUM)
	{
		LOG_ERROR("wdt", "Thread %s ID=%d is over MAX ID=%d", cThreadName, wWdtId, REQUEST_WDT_MAX_NUM);
		return false;
	}

	if (m_fReqWdtFlg[wWdtId] != 0)
	{
		fRet = true;
		m_fReqWdtFlg[wWdtId] = false;
		m_wWdtTimeout[wWdtId] = 0;
		m_wWdtCount[wWdtId] = 0;
		LOG_INFO("wdt", "Release thread %s ID=%d", cThreadName, wWdtId);
	}

	return fRet;
}

/**
 * @brief 喂狗操作，重置指定看门狗的计数器
 */
bool CSoftwareWdt::KeepSoftwareWdtAlive(int wWdtId)
{
	std::lock_guard<std::recursive_mutex> lock(mtx_);
	if (wWdtId < 0 || wWdtId >= REQUEST_WDT_MAX_NUM)
		return false;

	LOG_DEBUG("wdt", "Feed softdog ID=%d", wWdtId);
	m_wWdtCount[wWdtId] =0;

	return true;
}

/**
 * @brief 取消并等待指定线程结束（必须在锁外调用，避免死锁）
 */
static void cancelAndJoin(pthread_t tid, const char* name) {
    void* res;
    if (tid != 0 && pthread_kill(tid, 0) == 0) {
        if (pthread_cancel(tid) != 0) LOG_ERROR("wdt", "pthread_cancel %s failed", name);
        if (pthread_join(tid, &res) != 0) LOG_ERROR("wdt", "pthread_join %s failed", name);
        else if (res == PTHREAD_CANCELED) LOG_INFO("wdt", "%s was canceled", name);
    }
}

/**
 * @brief 监控所有注册线程的运行状态，超时则触发重启
 */
int CSoftwareWdt::MontiorWdtRunState()
{
	/* 第一阶段：持锁检测超时，收集需要取消的线程信息 */
	bool timeout_detected = false;
	bool trans_alive = false;
	{
		std::lock_guard<std::recursive_mutex> lock(mtx_);
		int bId;
		for (bId = 0; bId < REQUEST_WDT_MAX_NUM; bId++)
		{
			if (m_fReqWdtFlg[bId])
			{
				if (++m_wWdtCount[bId] > m_wWdtTimeout[bId])
				{
					LOG_WARN("wdt", "Wdt ID=%d timeout, count=%d, name=%s",
						bId, m_wWdtCount[bId], m_wwdtName[bId].c_str());
					if(m_wwdtName[bId] == "trans_message"){
						if (tid_transMessage == 0) continue;
						int status = pthread_kill(tid_transMessage, 0);
						if(status == EINVAL) LOG_ERROR("wdt", "pthread_kill EINVAL");
						timeout_detected = true;
						trans_alive = (status == 0);
						/* 释放所有已注册的看门狗ID */
						if (trans_alive) {
							for(int j = 0; j < REQUEST_WDT_MAX_NUM; j++){
								if (m_fReqWdtFlg[j]){
									this->ReleaseSoftwareWdtID(m_wwdtName[j].c_str(), j);
								}
							}
						}
					}
					break;
				}
			}
		}
	}
	/* 第二阶段：锁外执行 pthread_cancel + pthread_join，避免死锁 */
	if (timeout_detected) {
		cancelAndJoin(tid_getSensor, "getSensor");
		cancelAndJoin(tid_getLan, "getLan");
		cancelAndJoin(tid_gps, "gps");
		if (trans_alive) {
			cancelAndJoin(tid_transMessage, "transMessage");
		}
		return -1;
	}
	return 0;
}

/**
 * @brief 看门狗线程入口函数
 */
void* softwarewd(void* arg)
{
	CSoftwareWdt *g_CsoftwareWdt=(CSoftwareWdt *)arg;

	for(;;)
	{
		if(g_CsoftwareWdt->MontiorWdtRunState() == -1) {
			LOG_FATAL("wdt", "Thread timeout detected, watchdog exiting");
			return NULL;
		}
		sleep(4);
	}
}
