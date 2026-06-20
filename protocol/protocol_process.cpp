#include "protocol/protocol_process.h"
#include <vector>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
using namespace std;
extern string Isr_mac, id, net_id, mac, current_time;
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
#define STOREFILEPATHNAME       "/home/root/storefile.txt"

/** @brief 全局停止标志位，由信号处理器设置，主循环中检测 */
extern volatile sig_atomic_t g_stop_flag;

/**
 * @brief 程序退出处理函数，仅设置全局停止标志位
 * @param signum 信号编号（未使用，保留参数）
 * @note 实际清理逻辑在主循环中检测到标志位后执行
 */
void stop(int signum)
{
    (void)signum;
    g_stop_flag = 1;
}
/**
 * @brief 将0-15的整数转换为对应的十六进制字符
 * @param num 输入整数，有效范围0-15
 * @return 对应的十六进制字符字符串（"0"-"9"或"A"-"F"）
 */
string switch10_16(int num)
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
 * @param num 待转换的十进制数
 * @param str 引用参数，用于存储转换结果（高位在前）
 * @return 始终返回0
 */
int change(long long int num, string& str)
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
 * @brief 将整数转换为带一位小数的浮点数，用于气象仪数据格式转换
 * @param num 输入整数，最后一位为小数部分（如1234表示123.4）
 * @return 转换后的浮点数值
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
 * @brief 处理17协议（网关参数下发），提取ISR的MAC、设备ID、网络ID，并发送02确认包
 * @param buff 接收到的原始数据包字符串
 * @return 成功返回17，MAC不匹配或协议不匹配或写入失败返回-1
 */
int protoc_17(string buff)
{
    int ret;
    string confirm_message="";
    if (buff.length() < 54) return -1;
    string recv_protocal = buff.substr(1, 2);
    if (recv_protocal == REQ_SEND_INFO)
    {

        string recv_mac = buff.substr(16, 16);
        if (recv_mac == mac)
        {
            Isr_mac=buff.substr(32,16);
            id = buff.substr(48, 2);
            net_id = buff.substr(50, 4);
            confirm_message = confirm_message + "$" + "02" + "1" + id + net_id + "00" + "0012" + mac + id + "@"; 
            ret = write(fd_lora, confirm_message.c_str(),strlen(confirm_message.c_str()));
            if (ret <= 0)
            {
                printf("write confirm_message failed!\n");
                return -1;
            }else{
                if (ret < (int)strlen(confirm_message.c_str()))
                    printf("write confirm_message partial: %d/%zu bytes\n", ret, strlen(confirm_message.c_str()));
                cout << "confirm_message:" << confirm_message << endl;
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
 * @brief 处理03协议（数据请求），从数据包中提取通信方式
 * @param buff 接收到的原始数据包字符串
 * @param communicate_method 输出参数，用于存储提取的通信方式（4字符）
 * @return 成功返回3，MAC不匹配或协议不匹配返回-1
 */
int protoc_03(const string buff,string *communicate_method)
{
    if (buff.length() < 42) return -1;
    string recv_protocal = buff.substr(1, 2);
    if (recv_protocal == REQ_DATA)
    {
        string recv_mac = buff.substr(22, 16);
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
 * @brief 封装私有协议06包，包含通信类型、设备ID、网络ID、ISR的MAC、GPS、CPU内存等信息
 * @param strT 待发送的数据内容
 * @param portInfo 端口信息
 * @return 组装好的06协议数据包字符串，格式：$06+通信类型+ID+网络ID+00+长度+ISR_MAC+设备MAC+端口信息+GPS+CPU内存+通信状态+数据+@
 */
string packet06(string strT, string portInfo)
{
    char packet[1024]={0};
    string cpu_mem = get_cpuOccupy();
    string accessdev_gps = "N2932E10636";
    int length= (strT.length()+54);
    string str = ""; 
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
    string communicateType = to_string(CM->getSuccessId());
    int needed = snprintf(packet, sizeof(packet), "$06%s%s%s00%s%s%s%s%s%s%s%s@", communicateType.c_str(), id.c_str(), net_id.c_str(),
        str.c_str(), Isr_mac.c_str(), mac.c_str(), portInfo.c_str(), accessdev_gps.c_str(), cpu_mem.c_str(),
        communicate_status, strT.c_str());
    if (needed < 0) return "";
    if (needed >= (int)sizeof(packet)) {
        char* big_buf = new char[needed + 1];
        snprintf(big_buf, needed + 1, "$06%s%s%s00%s%s%s%s%s%s%s%s@", communicateType.c_str(), id.c_str(), net_id.c_str(),
            str.c_str(), Isr_mac.c_str(), mac.c_str(), portInfo.c_str(), accessdev_gps.c_str(), cpu_mem.c_str(),
            communicate_status, strT.c_str());
        string packets06(big_buf);
        delete[] big_buf;
        return packets06;
    }
    string packets06 = packet;
    return packets06;
}

/**
 * @brief 处理20协议（时间戳下发），从数据包中提取时间信息并更新系统时间
 * @param strPacket 引用参数，接收到的20协议数据包字符串
 */
void packet20(string& strPacket){
    cout<<"Start deal 20 time packet"<<endl;
    if (strPacket.length() < 49) return;
    string recv_mac = strPacket.substr(16, 16);
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
            cout << "mktime failed in packet20" << endl;
            return;
        }
        tv_set.tv_usec = atoi(current_time.substr(14, 3).c_str()) * 1000;
        settimeofday(&tv_set, NULL);
        cout<<"__________更新时间成功: "<<current_time<<"_________"<<endl;
    }
}

/**
 * @brief 将未发送的数据包队列写入本地文件进行持久化存储
 * @param buffer 数据包队列引用，写入后队列会被清空
 * @return 成功写入的数据包数量，失败时通过perror输出错误信息
 */
int dataTofile(MessageQueue<string>& buffer){
    int fd = open(STOREFILEPATHNAME, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR | S_IWUSR);
    if(fd < 0) { perror("in dataTofile open: "); return -1; }
    int count = 0;
    string item;
    while(buffer.tryPop(item)){
        const char* buff = item.c_str();
        int num = write(fd, buff, strlen(buff));
        if(num <= 0) { perror("in dataTofile write: "); continue; }
        if (write(fd, "\n", 1) < 0) perror("in dataTofile write newline: ");
        count ++;
    }
    close(fd);
    return count;
}

int drainAndPersist(MessageQueue<string>& buffer){
    vector<string> items;
    buffer.popAll(items);
    if(items.empty()) return 0;
    int fd = open(STOREFILEPATHNAME, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR | S_IWUSR);
    if(fd < 0) { perror("drainAndPersist open: "); return -1; }
    int count = 0;
    for(const auto& item : items){
        int num = write(fd, item.c_str(), item.length());
        if(num <= 0) { perror("drainAndPersist write: "); continue; }
        if (write(fd, "\n", 1) < 0) perror("drainAndPersist write newline: ");
        count++;
    }
    close(fd);
    return count;
}

int drainAndPersistUnsafe(MessageQueue<string>& buffer){
    vector<string> items;
    buffer.drainUnsafe(items);
    if(items.empty()) return 0;
    int fd = open(STOREFILEPATHNAME, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR | S_IWUSR);
    if(fd < 0) { perror("drainAndPersist open: "); return -1; }
    int count = 0;
    for(const auto& item : items){
        int num = write(fd, item.c_str(), item.length());
        if(num <= 0) { perror("drainAndPersistUnsafe write: "); continue; }
        if (write(fd, "\n", 1) < 0) perror("drainAndPersistUnsafe write newline: ");
        count++;
    }
    close(fd);
    return count;
}

/**
 * @brief 从本地文件读取未发送的数据包，并将06包转为21重发包
 * @param recvQueue 输出队列引用，用于存储读取并转换后的数据包
 * @return 读取的数据包数量，文件打开失败返回-1
 */
int readFromFile(queue<string>& recvQueue){
    FILE *fs = fopen(STOREFILEPATHNAME, "r");
    if (fs == NULL) {
        perror("fopen:");
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
        string s(buffer, len);
        cout << "未截取的字符串长度:" << s.length() << endl;
        /* 去除末尾'@'字符 */
        if (!s.empty() && s.back() == '@') {
            s.pop_back();
        }
        cout << "截取后的字符串长度:" << s.length() << endl;
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
    if(fd < 0) perror("in dataTofile open: ");
    else close(fd);
    return count;
}

