/**
 * @file hj212.cpp
 * @brief HJ212 协议工具函数实现
 * @details 提供 HJ212 协议数据包封装、CRC16 校验、时间戳生成等工具函数
 */
#include "protocol/hj212.h"
#include "infra/logger.h"
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

std::string ensureLen_4(int param)
{
    if (param < 0) {
        LOG_WARN("hj212", "ensureLen_4: negative param %d", param);
        return "0000";
    }
    std::string s = std::to_string(param);
    if (param < 10)
        return "000" + s;
    else if (param < 100)
        return "00" + s;
    else if (param < 1000)
        return "0" + s;
    else if (param < 10000)
        return s;
    else
    {
        LOG_WARN("hj212", "ensureLen_4: data overflow %d", param);
        return s;
    }
}

std::string ensure_crc4_packet(unsigned int crc, const std::string& len_str, const std::string& data)
{
    // 使用栈缓冲区替代堆分配，避免 new/delete 碎片
    // 增大到2048字节，确保23个船载传感器的数据段（约600+字节）不会溢出
    char buf[2048];
    int total_len = sizeof(buf);

    if (crc > 4095)
        snprintf(buf, total_len, "##%s%s%x", len_str.c_str(), data.c_str(), crc);
    else if (crc > 255)
        snprintf(buf, total_len, "##%s%s0%x", len_str.c_str(), data.c_str(), crc);
    else if (crc > 15)
        snprintf(buf, total_len, "##%s%s00%x", len_str.c_str(), data.c_str(), crc);
    else
        snprintf(buf, total_len, "##%s%s000%x", len_str.c_str(), data.c_str(), crc);

    return std::string(buf);
}

unsigned int CRC16_Checkout(unsigned char* puchMsg, unsigned int usDataLen)
{
    unsigned int i, j, crc_reg, check;
    crc_reg = 0xFFFF;
    for (i = 0; i < usDataLen; i++)
    {
        crc_reg = (crc_reg >> 8) ^ puchMsg[i];
        for (j = 0; j < 8; j++)
        {
            check = crc_reg & 0x0001;
            crc_reg >>= 1;
            if (check == 0x0001)
            {
                crc_reg ^= 0xA001;
            }
        }
    }
    return crc_reg;
}

std::string get_MN()
{
    std::string temp = "0000000000000000";  // 默认值，避免空字符串导致协议包格式错误
    char c[100] = {0};
    FILE* fptr;
    if ((fptr = fopen("./hj_mn.txt", "r")) == NULL)
    {
        LOG_ERROR("hj212", "Error opening hj_mn.txt, using default MN");
        return temp;
    }
    if (fscanf(fptr, "%99[^\n]", c) != 1) {
        LOG_ERROR("hj212", "Error reading MN from file, using default MN");
        fclose(fptr);
        return temp;
    }
    temp = c;
    fclose(fptr);
    return temp;
}

void string_formater(int source, char* dest)
{
    snprintf(dest, 3, "%02d", source);
}

void string_formater_usec(int source, char* dest)
{
    snprintf(dest, 4, "%03d", source);
}

std::string time_now_to_string()
{
    struct timeval tv;
    struct tm tm_buf;
    char timestamp_now[100];
    char stringyear[8], stringmonth[3], stringday[3], stringhour[3], stringmin[3], stringsec[3], stringusec[4];

    gettimeofday(&tv, NULL);
    if (localtime_r(&tv.tv_sec, &tm_buf) == NULL) {
        return "";
    }
    struct tm* p = &tm_buf;
    snprintf(stringyear, sizeof(stringyear), "%d", (p->tm_year + 1900));
    string_formater(p->tm_mon + 1, stringmonth);
    string_formater(p->tm_mday, stringday);
    string_formater(p->tm_hour, stringhour);
    string_formater(p->tm_min, stringmin);
    string_formater(p->tm_sec, stringsec);
    string_formater_usec(tv.tv_usec / 1000, stringusec);

    snprintf(timestamp_now, sizeof(timestamp_now), "%s%s%s%s%s%s%s",
             stringyear, stringmonth, stringday, stringhour, stringmin, stringsec, stringusec);

    return std::string(timestamp_now);
}
