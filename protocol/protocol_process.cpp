#include "protocol/protocol_process.h"
#include "infra/logger.h"
#include "infra/config.h"
#include <vector>
#include <time.h>
#include <sys/time.h>
#include <signal.h>

extern std::string Isr_mac, id, net_id, mac, current_time;
extern char communicate_status[];
extern communicaManage* CM;
extern int fd_lora;
#define REQ_REGISTER            "01"
#define REQ_REG_CONFIRM         "02"
#define REQ_DATA                 "03"
#define REQ_TXD_CONFIRM         "04"
#define REQ_PARAM_CONFIRM       "05"
#define REQ_PARAM_SEND          "06"
#define REQ_ID_CONFIRM          "07"
#define REQ_ID_SEND             "08"
#define REQ_MONI_RET_CONFIRM    "09"
#define REQ_MONI_RET            "10"
#define REQ_ACC_RET_CONFIRM     "11"
#define REQ_ACC_RET             "12"
#define REQ_KEY_CONFIRM         "13"
#define REQ_KEY_SEND            "14"
#define REQ_ASK_KEY             "15"
#define REQ_SEND_KEY            "16"
#define REQ_SEND_INFO           "17"
#define REQ_ACC_CLOSE           "18"
#define REQ_GATEWAY_CLOSE       "19"
#define TIME_SEND               "20"
#define REQ_ACC_HEART           "EF"
#define REQ_GATE_HEART          "FF"
#define STOREFILEPATHNAME       CFG_STR("data", "store_file", "/home/root/storefile.txt").c_str()

/** @brief 全局停止标志位，由信号处理器设置，主循环中检测 */
extern volatile sig_atomic_t g_stop_flag;

/**
 * @brief 程序退出处理函数，仅设置全局停止标志位
 */
void stop(int signum)
{
    (void)signum;
    g_stop_flag = 1;
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
 * @brief 将整数转换为带一位小数的浮点数
 */
float int2pString(int num)
{
    int a = num % 10;
    int b = num / 10;
    char buf[32];
    snprintf(buf, sizeof(buf), "%d.%d", b, a);
    float f = atof(buf);
    return f;
}

/**
 * @brief 处理17协议（网关参数下发）
 */
int protoc_17(std::string buff)
{
    int ret;
    std::string confirm_message="";
    if (buff.length() < 54) return -1;
    std::string recv_protocal = buff.substr(1, 2);
    if (recv_protocal == REQ_SEND_INFO)
    {

        std::string recv_mac = buff.substr(16, 16);
        if (recv_mac == mac)
        {
            Isr_mac=buff.substr(32,16);
            id = buff.substr(48, 2);
            net_id = buff.substr(50, 4);
            confirm_message = confirm_message + "$" + "02" + "1" + id + net_id + "00" + "0012" + mac + id + "@";
            ret = write(fd_lora, confirm_message.c_str(),strlen(confirm_message.c_str()));
            if (ret <= 0)
            {
                LOG_ERROR("protocol", "write confirm_message failed!");
                return -1;
            }else{
                if (ret < (int)strlen(confirm_message.c_str()))
                    LOG_WARN("protocol", "write confirm_message partial: %d/%zu bytes", ret, strlen(confirm_message.c_str()));
                LOG_INFO("protocol", "confirm_message: %s", confirm_message.c_str());
                return 17;
            }

        }else
        {

            return -1;
        }
    }else
    {
        return -1;
    }
}

/**
 * @brief 处理03协议（数据请求）
 */
int protoc_03(const std::string buff, std::string *communicate_method)
{
    if (buff.length() < 42) return -1;
    std::string recv_protocal = buff.substr(1, 2);
    if (recv_protocal == REQ_DATA)
    {
        std::string recv_mac = buff.substr(22, 16);
        if (recv_mac == mac)
        {
            *communicate_method = buff.substr(38, 4);
        }else
        {
            return -1;
        }

    }else
    {
        return -1;
    }
    return 3;
}

/**
 * @brief 封装私有协议06包
 */
std::string packet06(std::string strT, std::string portInfo)
{
    char packet[1024]={0};
    std::string cpu_mem = get_cpuOccupy();
    std::string accessdev_gps = "N2932E10636";
    int length= (strT.length()+54);
    std::string str = "";
    change(length, str);
    /* 长度字段确保4位十六进制，不足4位时前补0 */
    int num=str.length();
    if(num==4)
    {

    }else if(num==3)
    {
        str= "0" +str;
    }else if(num==2)
    {
        str="00" +str;
    }else if(num==1)
    {
        str="000"+str;
    }

    if (CM == NULL) return "";
    std::string communicateType = std::to_string(CM->getSuccessId());
    int needed = snprintf(packet, sizeof(packet), "$06%s%s%s00%s%s%s%s%s%s%s%s@", communicateType.c_str(), id.c_str(), net_id.c_str(),
        str.c_str(), Isr_mac.c_str(), mac.c_str(), portInfo.c_str(), accessdev_gps.c_str(), cpu_mem.c_str(),
        communicate_status, strT.c_str());
    if (needed < 0) return "";
    if (needed >= (int)sizeof(packet)) {
        char* big_buf = new char[needed + 1];
        snprintf(big_buf, needed + 1, "$06%s%s%s00%s%s%s%s%s%s%s%s@", communicateType.c_str(), id.c_str(), net_id.c_str(),
            str.c_str(), Isr_mac.c_str(), mac.c_str(), portInfo.c_str(), accessdev_gps.c_str(), cpu_mem.c_str(),
            communicate_status, strT.c_str());
        std::string packets06(big_buf);
        delete[] big_buf;
        return packets06;
    }
    std::string packets06 = packet;
    return packets06;
}

/**
 * @brief 处理20协议（时间戳下发）
 */
void packet20(std::string& strPacket){
    LOG_INFO("protocol", "Start deal 20 time packet");
    if (strPacket.length() < 49) return;
    std::string recv_mac = strPacket.substr(16, 16);
    if(recv_mac==mac )
    {
        current_time = strPacket.substr(32, 17);
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
            LOG_ERROR("protocol", "mktime failed in packet20");
            return;
        }
        tv_set.tv_usec = atoi(current_time.substr(14, 3).c_str()) * 1000;
        settimeofday(&tv_set, NULL);
        LOG_INFO("protocol", "Update time success: %s", current_time.c_str());
    }
}

/**
 * @brief 将未发送的数据包队列写入本地文件进行持久化存储
 */
int dataTofile(MessageQueue<std::string>& buffer){
    int fd = open(STOREFILEPATHNAME, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR | S_IWUSR);
    if(fd < 0) { LOG_ERROR("protocol", "dataTofile open failed: %s", strerror(errno)); return -1; }
    int count = 0;
    std::string item;
    while(buffer.tryPop(item)){
        const char* buff = item.c_str();
        int num = write(fd, buff, strlen(buff));
        if(num <= 0) { LOG_ERROR("protocol", "dataTofile write failed: %s", strerror(errno)); continue; }
        if (write(fd, "\n", 1) < 0) LOG_ERROR("protocol", "dataTofile write newline failed");
        count ++;
    }
    close(fd);
    return count;
}

int drainAndPersist(MessageQueue<std::string>& buffer){
    std::vector<std::string> items;
    buffer.popAll(items);
    if(items.empty()) return 0;
    int fd = open(STOREFILEPATHNAME, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR | S_IWUSR);
    if(fd < 0) { LOG_ERROR("protocol", "drainAndPersist open failed: %s", strerror(errno)); return -1; }
    int count = 0;
    for(const auto& item : items){
        int num = write(fd, item.c_str(), item.length());
        if(num <= 0) { LOG_ERROR("protocol", "drainAndPersist write failed: %s", strerror(errno)); continue; }
        if (write(fd, "\n", 1) < 0) LOG_ERROR("protocol", "drainAndPersist write newline failed");
        count++;
    }
    close(fd);
    return count;
}

int drainAndPersistUnsafe(MessageQueue<std::string>& buffer){
    std::vector<std::string> items;
    buffer.drainUnsafe(items);
    if(items.empty()) return 0;
    int fd = open(STOREFILEPATHNAME, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR | S_IWUSR);
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
    FILE *fs = fopen(STOREFILEPATHNAME, "r");
    if (fs == NULL) {
        LOG_ERROR("protocol", "fopen %s failed: %s", STOREFILEPATHNAME, strerror(errno));
        return -1;
    }
    char* buffer = NULL;
    size_t num = 0;
    int count = 0;
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
    int fd = open(STOREFILEPATHNAME, O_WRONLY | O_TRUNC);
    if(fd < 0) LOG_ERROR("protocol", "truncate storefile failed: %s", strerror(errno));
    else close(fd);
    return count;
}
