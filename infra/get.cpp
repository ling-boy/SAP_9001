#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <iostream>
#include <unistd.h>
#include "infra/get.h"

using namespace std;

/**
 * @brief 从/proc/meminfo读取系统内存使用信息
 *
 * 读取Linux系统/proc/meminfo文件的前5行，解析并存储
 * MemTotal、MemFree、Buffers、Cached和SwapCached的值。
 *
 * @param mem 输出参数，指向MEM_OCCUPY结构体，用于存储内存信息
 */
void get_memoccupy(MEM_OCCUPY *mem)
{
    FILE *fd;
    char buff[256];
    MEM_OCCUPY *m;
    m = mem;
    fd = fopen("/proc/meminfo", "r");
    if (fd == NULL) { perror("fopen /proc/meminfo"); return; }
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
 *
 * 读取Linux系统/proc/stat文件的第一行，解析CPU总体使用情况，
 * 包括user、nice、system、idle、lowait、irq和softirq。
 *
 * @param cpust 输出参数，指向CPU_OCCUPY结构体，用于存储CPU信息
 * @return 固定返回0
 */
int get_cpuoccupy(CPU_OCCUPY *cpust)
{
    FILE *fd;
    char buff[256];
    CPU_OCCUPY *cpu_occupy;
    cpu_occupy = cpust;
    fd = fopen("/proc/stat", "r");
    if (fd == NULL) { perror("fopen /proc/stat"); return -1; }
    if (fgets(buff, sizeof(buff), fd) == NULL) { fclose(fd); return -1; }
    sscanf(buff, "%19s %u %u %u %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice, &cpu_occupy->system, &cpu_occupy->idle, &cpu_occupy->lowait, &cpu_occupy->irq, &cpu_occupy->softirq);
    fclose(fd);
    return 0;
}

/**
 * @brief 计算内存使用率百分比
 *
 * 根据MemFree和MemTotal计算空闲内存占比，转换为2位字符串。
 * 例如："01"表示约1%空闲，"99"表示约99%空闲。
 * 注意：返回值是空闲率，不是使用率。
 *
 * @param m 指向包含内存信息的MEM_OCCUPY结构体
 * @return 2位字符串表示的内存空闲百分比（"01"~"99"）
 */
string cal_memoccupy(MEM_OCCUPY *m)
{
	if (m == NULL) return "01";
	double mem_use = 0;
	int mem_d;
	string mem;
	if (m->MemTotal == 0) return "01";
	mem_use=m->MemFree * 1.0 / (m->MemTotal * 1.0);
	mem_d = mem_use * 100;
	if (mem_d > 0) {
		if (mem_d < 10)
		{
			string str1 = to_string(mem_d);
			mem = "0" + str1;
		}
		else if (10 <= mem_d && mem_d < 100) {
			string str2 = to_string(mem_d);
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
 *
 * 比较两次采样的CPU时间片差异，计算user+nice+system占总时间的比例，
 * 转换为2位字符串。例如："01"~"99"。
 *
 * @param o 第一次采样的CPU_OCCUPY结构体（旧数据）
 * @param n 第二次采样的CPU_OCCUPY结构体（新数据）
 * @return 2位字符串表示的CPU使用率百分比（"01"~"99"）
 */
string cal_cpuoccupy(CPU_OCCUPY *o, CPU_OCCUPY *n)
{
    if (o == NULL || n == NULL) return "01";
    unsigned long od, nd;
    double cpu_use = 0;
	int cpu_d;
	string cpu_r;
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
			string str1 = to_string(cpu_d);
			cpu_r = "0" + str1;
		}
		else if (10 <= cpu_d && cpu_d<100) {
			string str2 = to_string(cpu_d);
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
 *
 * 通过两次CPU采样（间隔100ms）计算CPU使用率，并读取内存信息，
 * 返回4位组合字符串：前2位为CPU使用率，后2位为空闲内存率。
 * 例如："5020"表示CPU使用率50%，内存空闲率20%。
 *
 * @return 4位字符串，格式为"CPU使用率(2位)+内存空闲率(2位)"
 */
string get_cpuOccupy(){
    MEM_OCCUPY mem_stat;
    CPU_OCCUPY cpu_stat1;
    CPU_OCCUPY cpu_stat2;
    string mem;
    string cpu;
    string mem_cpu;
    string cpu_mem;
    get_memoccupy(&mem_stat);
    mem = cal_memoccupy(&mem_stat);

    if (get_cpuoccupy((CPU_OCCUPY *)&cpu_stat1) < 0)
        return "0101";
    /* 采样间隔100ms，用于计算CPU使用率差值 */
    usleep(100000);
    if (get_cpuoccupy((CPU_OCCUPY *)&cpu_stat2) < 0)
        return "0101";
    cpu =cal_cpuoccupy(&cpu_stat1, &cpu_stat2);
    cpu_mem=cpu+mem;
    return cpu_mem;
}
