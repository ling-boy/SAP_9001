#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include <cerrno>
#include "infra/get.h"
#include "infra/logger.h"

/**
 * @brief 从/proc/meminfo读取系统内存使用信息
 */
void get_memoccupy(MEM_OCCUPY *mem)
{
    FILE *fd;
    char buff[256];
    MEM_OCCUPY *m;
    m = mem;
    fd = fopen("/proc/meminfo", "r");
    if (fd == NULL) { LOG_ERROR("sysmon", "fopen /proc/meminfo: %s", strerror(errno)); return; }
    if (fgets(buff, sizeof(buff), fd) == NULL) { fclose(fd); return; }
    sscanf(buff, "%19s %lu ", m->name1, &m->MemTotal);
    if (fgets(buff, sizeof(buff), fd) == NULL) { fclose(fd); return; }
    sscanf(buff, "%19s %lu ", m->name2, &m->MemFree);
    if (fgets(buff, sizeof(buff), fd) == NULL) { fclose(fd); return; }
    sscanf(buff, "%19s %lu ", m->name3, &m->Buffers);
    if (fgets(buff, sizeof(buff), fd) == NULL) { fclose(fd); return; }
    sscanf(buff, "%19s %lu ", m->name4, &m->Cached);
    if (fgets(buff, sizeof(buff), fd) == NULL) { fclose(fd); return; }
    sscanf(buff, "%19s %lu", m->name5, &m->SwapCached);
    fclose(fd);
}


/**
 * @brief 从/proc/stat读取CPU使用信息
 */
int get_cpuoccupy(CPU_OCCUPY *cpust)
{
    FILE *fd;
    char buff[256];
    CPU_OCCUPY *cpu_occupy;
    cpu_occupy = cpust;
    fd = fopen("/proc/stat", "r");
    if (fd == NULL) { LOG_ERROR("sysmon", "fopen /proc/stat: %s", strerror(errno)); return -1; }
    if (fgets(buff, sizeof(buff), fd) == NULL) { fclose(fd); return -1; }
    sscanf(buff, "%19s %u %u %u %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice, &cpu_occupy->system, &cpu_occupy->idle, &cpu_occupy->lowait, &cpu_occupy->irq, &cpu_occupy->softirq);
    fclose(fd);
    return 0;
}

/**
 * @brief 计算内存使用率百分比
 */
std::string cal_memoccupy(MEM_OCCUPY *m)
{
	if (m == NULL) return "01";
	double mem_use = 0;
	int mem_d;
	std::string mem;
	if (m->MemTotal == 0) return "01";
	mem_use=(m->MemTotal - m->MemFree) * 1.0 / (m->MemTotal * 1.0);
	mem_d = mem_use * 100;
	if (mem_d > 0) {
		if (mem_d < 10)
		{
			std::string str1 = std::to_string(mem_d);
			mem = "0" + str1;
		}
		else if (10 <= mem_d && mem_d < 100) {
			std::string str2 = std::to_string(mem_d);
			mem = str2;
		}
		else {
			mem = "99";
		}
	}
	else {
		mem = "01";
	}
	return mem;
}

/**
 * @brief 通过两次CPU采样计算CPU使用率百分比
 */
std::string cal_cpuoccupy(CPU_OCCUPY *o, CPU_OCCUPY *n)
{
    if (o == NULL || n == NULL) return "01";
    unsigned long od, nd;
    double cpu_use = 0;
	int cpu_d;
	std::string cpu_r;
    od = (unsigned long)o->user + (unsigned long)o->nice + (unsigned long)o->system + (unsigned long)o->idle + (unsigned long)o->lowait + (unsigned long)o->irq + (unsigned long)o->softirq;
    nd = (unsigned long)n->user + (unsigned long)n->nice + (unsigned long)n->system + (unsigned long)n->idle + (unsigned long)n->lowait + (unsigned long)n->irq + (unsigned long)n->softirq;
    double sum = nd - od;
    if (sum == 0) return "01";
    /* 计算实际CPU工作时间(user+nice+system) */
    double busy = n->user + n->system + n->nice - o->user - o->system - o->nice;
    cpu_use = busy / sum;
	cpu_d = cpu_use * 100;

	if (cpu_d>0) {
		if (cpu_d < 10)
		{
			std::string str1 = std::to_string(cpu_d);
			cpu_r = "0" + str1;
		}
		else if (10 <= cpu_d && cpu_d<100) {
			std::string str2 = std::to_string(cpu_d);
			cpu_r = str2;

		}
		else {
			cpu_r = "99";
		}
	}
	else {
		cpu_r = "01";
	}
	return cpu_r;
}


/**
 * @brief 获取CPU和内存使用率的组合字符串
 * @details 线程安全版本，使用互斥锁保护静态变量
 *          优化：先检查缓存（持锁），采样时释放锁避免阻塞其他线程
 *
 * @note **缓存竞争说明**：采样阶段（约100ms usleep）未持锁，
 *       多个线程可能同时进入采样路径并重复计算。
 *       这是刻意的设计权衡：避免持锁100ms阻塞心跳线程。
 *       最后一个完成采样的线程会更新缓存，结果对心跳包精度足够。
 *       若需要严格单次采样，可使用 std::call_once 或 atomic flag。
 */
std::string get_cpuOccupy(){
    static std::string cached_result;
    static time_t last_sample_time = 0;
    static std::mutex cpu_mtx;

    // 第一阶段：持锁检查缓存
    {
        std::lock_guard<std::mutex> lock(cpu_mtx);
        time_t now = time(NULL);
        if (!cached_result.empty() && (now - last_sample_time) < 30) {
            return cached_result;
        }
    }

    // 第二阶段：释放锁后执行采样（避免持锁阻塞100ms）
    MEM_OCCUPY mem_stat;
    CPU_OCCUPY cpu_stat1;
    CPU_OCCUPY cpu_stat2;
    get_memoccupy(&mem_stat);
    std::string mem = cal_memoccupy(&mem_stat);

    if (get_cpuoccupy((CPU_OCCUPY *)&cpu_stat1) < 0)
        return "0101";
    usleep(100000);
    if (get_cpuoccupy((CPU_OCCUPY *)&cpu_stat2) < 0)
        return "0101";
    std::string cpu = cal_cpuoccupy(&cpu_stat1, &cpu_stat2);
    std::string cpu_mem = cpu + mem;

    // 第三阶段：持锁更新缓存
    {
        std::lock_guard<std::mutex> lock(cpu_mtx);
        cached_result = cpu_mem;
        last_sample_time = time(NULL);
    }

    return cpu_mem;
}
