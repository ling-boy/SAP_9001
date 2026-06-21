/**
 * @file gps.cpp
 * @brief GPS 数据采集模块
 * @details 通过串口 /dev/ttymxc7 读取 NMEA $GNRMC 语句，
 *          解析经纬度信息，提供线程安全的访问接口
 */
#include "hal/gps.h"
#include "hal/usbctl.h"
#include "infra/logger.h"
#include "infra/config.h"
#include "core/device_context.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <mutex>

/* GPS 数据内部存储（模块私有） */
static std::string s_gps_lat = "NFFFF";
static std::string s_gps_lon = "EFFFFF";
static std::mutex s_mutex_gps;

/**
 * @brief 解析GPS数据，提取经纬度信息
 * @param line      GPS原始数据字符串
 * @param lat       存储纬度信息
 * @param flag_lat  存储纬度标志（N/S）
 * @param lon       存储经度信息
 * @param flag_lon  存储经度标志（E/W）
 * @return 0 成功，-1 失败
 */
static int process_gps(char* line, char* lat, int lat_size, char* flag_lat, int flag_lat_size,
                       char* lon, int lon_size, char* flag_lon, int flag_lon_size)
{
    char* token;
    token = strtok(line, ",");
    if (token == NULL) return -1;
    /* 跳过时间字段，检查状态字段：V=无效定位 */
    char* time_field = strtok(NULL, ",");
    char* status = strtok(NULL, ",");
    if (status != NULL && status[0] == 'V')
    {
        return -1;
    }
    /* 继续解析经纬度字段（strtok 已消耗 $GNRMC, time, status 三个字段） */
    if (token != NULL && !strcmp(token, "$GNRMC"))
    {
        char* latitude = strtok(NULL, ",");   // 纬度
        char* ns_flag = strtok(NULL, ",");    // N/S 标志
        char* longitude = strtok(NULL, ",");  // 经度
        char* ew_flag = strtok(NULL, ",");    // E/W 标志
        if (latitude == NULL || ns_flag == NULL || longitude == NULL || ew_flag == NULL) return -1;
        strncpy(lat, latitude, lat_size - 1); lat[lat_size - 1] = '\0';
        strncpy(flag_lat, ns_flag, flag_lat_size - 1); flag_lat[flag_lat_size - 1] = '\0';
        strncpy(lon, longitude, lon_size - 1); lon[lon_size - 1] = '\0';
        strncpy(flag_lon, ew_flag, flag_lon_size - 1); flag_lon[flag_lon_size - 1] = '\0';
    }
    else {
        return -1;
    }
    return 0;
}

/**
 * @brief GPS fd 清理函数（pthread_cleanup 使用）
 */
static void gps_fd_cleanup(void* arg) {
    int fd = *static_cast<int*>(arg);
    if (fd >= 0) {
        close(fd);
        LOG_INFO("gps", "GPS fd %d closed by cleanup handler", fd);
    }
}

/**
 * @brief GPS数据获取线程
 */
void* GET_GPS(void* arg)
{
    (void)arg;
    LOG_INFO("gps", "Start GPS thread");
    std::string gps_script = CFG_STR("paths", "gps_script", "/home/root/gps_test.sh");
    std::string gps_cmd = gps_script + " &";
    if (system(gps_cmd.c_str()) < 0) {
        LOG_WARN("gps", "system() failed to launch gps_test.sh");
    }
    sleep(60);
    int ret;
    std::string gps_port = CFG_STR("serial.gps", "port", "/dev/ttymxc7");
    char buf[300] = {0};
    char f_lat[5];
    char f_lon[5];
    char lat[10];
    char lon[12];
    int fd = -1;

    fd = open(gps_port.c_str(), O_RDWR | O_NOCTTY);
    if (-1 == fd)
    {
        LOG_ERROR("gps", "Open GPS device error! GPS functionality disabled.");
        // 设置 GPS 失败标志，通知主线程
        auto& ctx = sap::DeviceContext::instance();
        ctx.fds().gps_failed = true;
        return NULL;
    }

    // 注册清理函数，确保 fd 在 pthread_cancel 时被关闭
    pthread_cleanup_push(gps_fd_cleanup, &fd);

    {
        LOG_INFO("gps", "Open GPS device success");
        ret = set_opt1(fd, 9600, 8, 'n', 1);
        if (ret < 0) {
            LOG_ERROR("gps", "set_opt1 failed");
            close(fd);
            return NULL;
        }
        int flag = 0;
        char in;
        int i = 0;
        int flag_read = 0;
        while (1)
        {
            while (1)
            {
                ssize_t n = read(fd, &in, 1);
                if (n <= 0) break;
                if (in != '$')
                {
                    if (0 == flag_read) continue;
                    else if (in == '\n' || in == '*')
                    {
                        buf[i] = '\0';
                        flag_read = 0;
                        i = 0;
                        break;
                    }
                    else if (flag_read == 1 && in != '\n' && i < (int)sizeof(buf) - 1) buf[i++] = in;
                }
                else if (in == '$' && flag_read == 0)
                {
                    flag_read = 1;
                    i = 0;
                    buf[i++] = in;
                }
            }
            /* 未读到完整行（read失败或中途断开），跳过解析 */
            if (flag_read == 1) {
                flag_read = 0;
                i = 0;
                memset(buf, 0, sizeof(buf));
                sleep(2);
                continue;
            }
            flag = process_gps(buf, lat, sizeof(lat), f_lat, sizeof(f_lat), lon, sizeof(lon), f_lon, sizeof(f_lon));
            if (-1 == flag)
            {
                continue;
            }
            else {
                int lat_len = strlen(lat);
                int lon_len = strlen(lon);
                std::string s_lon = "";
                for (int j = 0; j < lon_len; j++) {
                    if (j == 5) continue; /* 跳过小数点 */
                    s_lon += lon[j];
                }
                std::string s_lat = "";
                for (int j = 0; j < lat_len; j++) {
                    if (j == 4) continue; /* 跳过小数点 */
                    s_lat += lat[j];
                }

                {
                    std::lock_guard<std::mutex> lock(s_mutex_gps);
                    s_gps_lat = std::string(f_lat) + s_lat;
                    s_gps_lon = std::string(f_lon) + s_lon;
                }
            }
            memset(buf, 0, sizeof(buf));
            sleep(2);
        }
    }

    // 执行清理（关闭 fd）
    pthread_cleanup_pop(1);
    return NULL;
}

std::string gps_get_latitude()
{
    std::lock_guard<std::mutex> lock(s_mutex_gps);
    return s_gps_lat;
}

std::string gps_get_longitude()
{
    std::lock_guard<std::mutex> lock(s_mutex_gps);
    return s_gps_lon;
}

std::string gps_get_location()
{
    std::lock_guard<std::mutex> lock(s_mutex_gps);
    return s_gps_lat + s_gps_lon;
}
