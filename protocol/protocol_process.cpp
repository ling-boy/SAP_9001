#include "protocol/protocol_process.h"
#include "protocol/hj212_builder.h"
#include "protocol/constants.h"
#include "infra/config.h"
#include "core/device_context.h"
#include <vector>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
// logger.h must be included AFTER all other headers to override syslog.h LOG_INFO
#include "infra/logger.h"

// 使用内联函数替代宏，避免 .c_str() 悬空指针风险
static std::string getStoreFilePath() {
    return CFG_STR("data", "store_file", "/home/root/storefile.txt");
}

/**
 * @brief 将0-15的整数转换为对应的十六进制字符
 */
std::string switch10_16(int num)
{
    switch(num){
        case 0:return"0";
        case 1:return"1";
        case 2:return"2";
        case 3:return"3";
        case 4:return"4";
        case 5:return"5";
        case 6:return"6";
        case 7:return"7";
        case 8:return"8";
        case 9:return"9";
        case 10:return"A";
        case 11:return"B";
        case 12:return"C";
        case 13:return"D";
        case 14:return"E";
        case 15:return"F";
        default:return"0";
    }
}

/**
 * @brief 递归将十进制数转换为十六进制字符串
 */
int change(long long int num, std::string& str)
{
    if(num<16)
    {
        str+=switch10_16(num);
        return 0;
    }
    if(num>=16)
    {
        change(num/16,str);
        str+=switch10_16(num%16);
    }
    return 0;
}

/**
 * @brief 封装私有协议06包（使用 HJ212PacketBuilder 建造者模式）
 */
std::string packet06(std::string strT, std::string portInfo)
{
    return sap::HJ212PacketBuilder()
        .fromContext()
        .withData(strT)
        .withPortInfo(portInfo)
        .build();
}

/**
 * @brief 处理20协议（时间戳下发）
 */
void packet20(std::string& strPacket){
    auto& ctx = sap::DeviceContext::instance();
    LOG_INFO("protocol", "%s", "Start deal 20 time packet");
    if (strPacket.length() < 49) return;
    std::string recv_mac = strPacket.substr(16, 16);
    if(recv_mac==ctx.getIdentityMac() )
    {
        std::string current_time = strPacket.substr(32, 17);
        ctx.setIdentityCurrentTime(current_time);
        struct tm tm_set;
        memset(&tm_set, 0, sizeof(tm_set));
        tm_set.tm_year = atoi(current_time.substr(0, 4).c_str()) - 1900;
        tm_set.tm_mon  = atoi(current_time.substr(4, 2).c_str()) - 1;
        tm_set.tm_mday = atoi(current_time.substr(6, 2).c_str());
        tm_set.tm_hour = atoi(current_time.substr(8, 2).c_str());
        tm_set.tm_min  = atoi(current_time.substr(10, 2).c_str());
        tm_set.tm_sec  = atoi(current_time.substr(12, 2).c_str());
        struct timeval tv_set;
        tv_set.tv_sec = mktime(&tm_set);
        if (tv_set.tv_sec == (time_t)-1) {
            LOG_ERROR("protocol", "%s", "mktime failed in packet20");
            return;
        }
        tv_set.tv_usec = atoi(current_time.substr(14, 3).c_str()) * 1000;
        settimeofday(&tv_set, NULL);
        LOG_INFO("protocol", "Update time success: %s", current_time.c_str());
    }
}

int drainAndPersist(MessageQueue<std::string>& buffer){
    std::vector<std::string> items;
    buffer.popAll(items);
    if(items.empty()) return 0;
    int fd = open(getStoreFilePath().c_str(), O_WRONLY|O_CREAT|O_APPEND, S_IRUSR | S_IWUSR);
    if(fd < 0) { LOG_ERROR("protocol", "drainAndPersist open failed: %s", strerror(errno)); return -1; }
    int count = 0;
    for(const auto& item : items){
        int num = write(fd, item.c_str(), item.length());
        if(num <= 0) { LOG_ERROR("protocol", "drainAndPersist write failed: %s", strerror(errno)); continue; }
        if (write(fd, "\n", 1) < 0) LOG_ERROR("protocol", "%s", "drainAndPersist write newline failed");
        count++;
    }
    close(fd);
    return count;
}

/**
 * @brief 从消息队列中取出所有数据并持久化到文件（非线程安全版本）
 * @warning **已知风险**：此函数调用 buffer.drainUnsafe() 而非线程安全的 popAll()，
 *          仅应在信号处理器（signal handler）等无法获取锁的上下文中使用。
 *          POSIX 信号处理器中只能调用 async-signal-safe 函数，
 *          此处使用了 open/write/close（信号安全）但 drainUnsafe 内部操作
 *          std::vector 移动语义不属于严格安全操作。
 *          实践中在嵌入式 Linux 单线程信号处理场景下风险可控，
 *          但不应在多线程信号处理或非信号上下文中调用此函数。
 */
int drainAndPersistUnsafe(MessageQueue<std::string>& buffer){
    std::vector<std::string> items;
    buffer.drainUnsafe(items);
    if(items.empty()) return 0;
    int fd = open(getStoreFilePath().c_str(), O_WRONLY|O_CREAT|O_APPEND, S_IRUSR | S_IWUSR);
    if(fd < 0) { perror("drainAndPersist open"); return -1; }
    int count = 0;
    for(const auto& item : items){
        int num = write(fd, item.c_str(), item.length());
        if(num <= 0) { perror("drainAndPersistUnsafe write"); continue; }
        if (write(fd, "\n", 1) < 0) perror("drainAndPersistUnsafe write newline");
        count++;
    }
    close(fd);
    return count;
}

/**
 * @brief 从本地文件读取未发送的数据包，并将06包转为21重发包
 */
int readFromFile(std::queue<std::string>& recvQueue){
    FILE *fs = fopen(getStoreFilePath().c_str(), "r");
    if (fs == NULL) {
        LOG_ERROR("protocol", "fopen %s failed: %s", getStoreFilePath().c_str(), strerror(errno));
        return 0;
    }
    char* buffer = NULL;
    size_t num = 0;
    int count = 0;
    /* @note getline 内部使用 realloc 管理 buffer 堆内存，
     *       每次循环结束后 free 并重置为 NULL 以避免内存泄漏。
     *       在嵌入式场景下，频繁的 malloc/free 可能导致堆碎片，
     *       但 getline 是 POSIX 标准接口，此处使用合理。 */
    while(getline(&buffer, &num, fs) != -1){
        size_t len = strlen(buffer);
        if (len == 0) { free(buffer); buffer = NULL; num = 0; continue; }
        /* 去除末尾换行符 */
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }
        if (len == 0) { free(buffer); buffer = NULL; num = 0; continue; }

        /* 将06包协议号改为21重发包：$06 -> $21 */
        if (len >= 3 && buffer[0] == '$') {
            buffer[1] = '2';
            buffer[2] = '1';
        }
        std::string s(buffer, len);
        LOG_DEBUG("protocol", "Read packet length: %zu", s.length());
        /* 去除末尾'@'字符 */
        if (!s.empty() && s.back() == '@') {
            s.pop_back();
        }
        LOG_DEBUG("protocol", "After trim length: %zu", s.length());
        recvQueue.push(s);

        count ++;
        free(buffer);
        buffer = NULL;
        num = 0;
    }
    if(buffer){
        free(buffer);
    }
    fclose(fs);
    int fd = open(getStoreFilePath().c_str(), O_WRONLY | O_TRUNC);
    if(fd < 0) LOG_ERROR("protocol", "truncate storefile failed: %s", strerror(errno));
    else close(fd);
    return count;
}

/**
 * @brief 构建心跳包（EF协议）
 * @details 心跳包包含设备状态信息：CPU使用率、内存空闲率、通信状态、队列深度
 *          格式：$EF + 通信类型 + 设备ID + 网络ID + 00 + 长度 + CPU + 内存 + 通信状态 + 队列深度 + @
 * @return 心跳包字符串
 */
std::string buildHeartbeat()
{
    auto& ctx = sap::DeviceContext::instance();

    // 获取系统状态
    std::string cpu_mem = get_cpuOccupy();
    int activeId = ctx.getActiveDeviceId();
    // 处理负数设备ID（无可用设备时返回-1）
    std::string communicateType = (activeId >= 0) ? std::to_string(activeId) : "0";

    // 获取队列深度
    int queue_depth = ctx.queues().transmit.size();

    // 构建数据段：CPU使用率(2位) + 内存空闲率(2位) + 通信状态(4位) + 队列深度(4位) + GPS状态(1位)
    char data_segment[20];
    std::string comm_status = ctx.getCommunicateStatus();
    int gps_ok = ctx.fds().gps_failed ? 0 : 1;
    snprintf(data_segment, sizeof(data_segment), "%s%s%04d%d",
             cpu_mem.c_str(), comm_status.c_str(), queue_depth, gps_ok);

    // 计算长度：数据段长度 + 固定字段长度(设备ID + 网络ID + 通信类型 = 2+4+1 = 7)
    int length = strlen(data_segment) + 7;
    char len_hex[5];
    snprintf(len_hex, sizeof(len_hex), "%04X", length);
    std::string len_str(len_hex);

    // 组装心跳包
    char packet[256];
    snprintf(packet, sizeof(packet), "$EF%s%s%s00%s%s@",
             communicateType.c_str(),
             ctx.getIdentityId().c_str(),
             ctx.getIdentityNetId().c_str(),
             len_str.c_str(),
             data_segment);

    LOG_DEBUG("protocol", "Heartbeat: %s", packet);
    return std::string(packet);
}
