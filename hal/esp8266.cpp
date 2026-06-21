#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>
#include <stdint.h>
#include "hal/usbctl.h"
#include "hal/esp8266.h"
#include <string.h>
#include <string>
#include "infra/logger.h"
#include "infra/config.h"

#define SERVERPORT 8080

#define USE_PROTOCOL USE_TCP
#define USE_UDP 0
#define USE_TCP 1

#define DELAY_SEC 1

/**
 * @brief 打开ESP8266 WiFi模块并验证设备是否正常响应
 * @note 预留接口，供后续扩展。当前系统主要使用 WiFi STA 模式。
 *
 * 通过串口打开ESP8266设备，发送AT指令检测设备是否在线，
 * 然后重启设备并等待重启完成标志。
 *
 * @return 成功返回串口文件描述符(fd)，失败返回-1
 */
int esp8266_open()
{
    int fd, ret;
    char* p;
    uint8_t buf[1024];

    struct timeval timeout;
    fd_set fdset;
    FD_ZERO(&fdset);

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    fd = open_port(1);
    if (-1 == fd)
    {
        LOG_ERROR("esp8266", "Init ESP8266 fail");
        return -1;
    }
    ret = set_opt1(fd, 115200, 8, 'n', 1);
    if (ret < 0) {
        LOG_ERROR("esp8266", "set_opt1 failed");
        close(fd);
        return -1;
    }
    FD_SET(fd, &fdset);
    /* 发送AT指令测试设备是否正常响应 */
    if (write_port(fd, (uint8_t*)"AT\r\n", 4) < 0) {
        LOG_ERROR("esp8266", "write AT failed");
        close(fd);
        return -1;
    }
    sleep(DELAY_SEC);
    ret = select(fd + 1, &fdset, NULL, NULL, &timeout);
    switch (ret)
    {
    case -1:
        LOG_ERROR("esp8266", "select() error");
        break;
    case 0:
        LOG_WARN("esp8266", "select() timeout");
        break;
    default:
        if (FD_ISSET(fd, &fdset))
        {
            ret = read_port(fd, buf, 300);
            if (ret > 0)
            {
                buf[ret] = '\0';
                p = strstr((char*)buf, "OK");
                if (p != NULL)
                {
                    /* 发送AT+RST指令重启ESP8266设备 */
                    if (write_port(fd, (uint8_t*)"AT+RST\r\n", 8) < 0) {
                        LOG_ERROR("esp8266", "write AT+RST failed");
                        close(fd);
                        return -1;
                    }
                    sleep(DELAY_SEC);
                    memset(buf, 0, sizeof(buf));
                    ret = read(fd, buf, 300);
                    if (ret > 0) buf[ret] = '\0';
                    p = strstr((char*)buf, "OK");
                    if (p == NULL)
                    {
                        close(fd);
                        LOG_ERROR("esp8266", "RST Failed");
                        return -1;
                    }
                    /* 持续读取数据，等待"jump"标志表示设备重启完成 */
                    {
                        std::string acc;
                        fd_set rset;
                        struct timeval wt;
                        int jump_found = 0;
                        for (int retry = 0; retry < 30 && !jump_found; retry++)
                        {
                            FD_ZERO(&rset);
                            FD_SET(fd, &rset);
                            wt.tv_sec = 1;
                            wt.tv_usec = 0;
                            int sr = select(fd + 1, &rset, NULL, NULL, &wt);
                            if (sr > 0 && FD_ISSET(fd, &rset))
                            {
                                ssize_t n = read(fd, buf, sizeof(buf) - 1);
                                if (n > 0)
                                {
                                    buf[n] = '\0';
                                    acc += (char*)buf;
                                    if (acc.find("jump") != std::string::npos)
                                    {
                                        jump_found = 1;
                                    }
                                }
                                else if (n <= 0)
                                {
                                    break;
                                }
                            }
                            else if (sr < 0)
                            {
                                break;
                            }
                        }
                        if (jump_found)
                        {
                            tcflush(fd, TCIFLUSH);
                            LOG_INFO("esp8266", "ESP8266 opened successfully");
                            return fd;
                        }
                    }
                }
            }
        }
    }
    FD_CLR(fd, &fdset);
    close(fd);
    return -1;
}

/**
 * @brief 配置ESP8266为AP热点模式并开启TCP服务器
 * @note 预留接口，供后续扩展。当前系统主要使用 WiFi STA 模式。
 *
 * 设置ESP8266工作在AP模式，热点名称、密码、端口等通过配置文件读取，
 * 开启多连接模式并在配置端口启动TCP服务器。
 *
 * @param fd 已打开的ESP8266串口文件描述符
 * @return 成功返回0
 */
int esp8266_config(int fd)
{
    uint8_t RST[] = "AT+RST\r\n";
    /* AT+CWMODE=2 设置ESP8266为AP(热点)模式 */
    uint8_t CWMODE[] = "AT+CWMODE=2\r\n";
    /* AT+CWSAP 配置热点名称和密码，格式: AT+CWSAP=<ssid>,<pwd>,<chl>,<ecn> */
    std::string ssid = CFG_STR("network.esp8266", "ssid", "g202001");
    std::string pwd = CFG_STR("network.esp8266", "password", "12345678");
    int channel = CFG_INT("network.esp8266", "channel", 1);
    int ecn = CFG_INT("network.esp8266", "ecn", 3);
    char cwsap_buf[128];
    snprintf(cwsap_buf, sizeof(cwsap_buf), "AT+CWSAP=\"%s\",\"%s\",%d,%d\r\n",
             ssid.c_str(), pwd.c_str(), channel, ecn);
    std::string CWSAP_CMD(cwsap_buf);
#if USE_PROTOCOL == USE_UDP
    uint8_t CIPSTART[] = "AT+CIPSTART=\"UDP\",\"192.168.4.255\",2345,2345,0\r\n";
    uint8_t CIPMODE[] = "AT+CIPMODE=1\r\n";
    uint8_t CIPSEND[] = "AT+CIPSEND\r\n";
#else
    /* AT+CIPMUX=1 开启多连接模式(最多5个客户端) */
    uint8_t CIPMUX[] = "AT+CIPMUX=1\r\n";
    /* AT+CIPSERVER=1,8080 在8080端口开启TCP服务器 */
    int server_port = CFG_INT("network.esp8266", "server_port", 8080);
    char cipserver_buf[64];
    snprintf(cipserver_buf, sizeof(cipserver_buf), "AT+CIPSERVER=1,%d\r\n", server_port);
    std::string CIPSERVER_CMD(cipserver_buf);
#endif

    if (set_opt1(fd, 115200, 8, 'n', 1) < 0) {
        LOG_ERROR("esp8266", "config set_opt1 failed");
        return -1;
    }
    if (write_port(fd, RST, strlen((const char*)RST)) < 0) {
        LOG_WARN("esp8266", "config write RST failed");
    }
    sleep(DELAY_SEC);
    /* 设置AP模式 */
    if (write_port(fd, CWMODE, strlen((const char*)CWMODE)) < 0) {
        LOG_WARN("esp8266", "config write CWMODE failed");
    }
    sleep(DELAY_SEC);
    if (write_port(fd, RST, strlen((const char*)RST)) < 0) {
        LOG_WARN("esp8266", "config write RST failed");
    }
    sleep(DELAY_SEC);
    /* 配置热点名称和密码 */
    if (write_port(fd, CWSAP_CMD.c_str(), CWSAP_CMD.size()) < 0) {
        LOG_WARN("esp8266", "config write CWSAP failed");
    }
    sleep(DELAY_SEC);
    if (write_port(fd, RST, strlen((const char*)RST)) < 0) {
        LOG_WARN("esp8266", "config write RST failed");
    }
    sleep(DELAY_SEC);
#if USE_PROTOCOL == USE_UDP
    if (write_port(fd, CIPSTART, strlen((const char*)CIPSTART)) < 0) {
        LOG_WARN("esp8266", "config write CIPSTART failed");
    }
    sleep(DELAY_SEC);
    if (write_port(fd, CIPMODE, strlen((const char*)CIPMODE)) < 0) {
        LOG_WARN("esp8266", "config write CIPMODE failed");
    }
    sleep(DELAY_SEC);
    if (write_port(fd, CIPSEND, strlen((const char*)CIPSEND)) < 0) {
        LOG_WARN("esp8266", "config write CIPSEND failed");
    }
    sleep(DELAY_SEC);
#else
    /* 开启多连接模式 */
    if (write_port(fd, CIPMUX, strlen((const char*)CIPMUX)) < 0) {
        LOG_WARN("esp8266", "config write CIPMUX failed");
    }
    sleep(DELAY_SEC);
    /* 开启TCP服务器，监听配置端口 */
    if (write_port(fd, CIPSERVER_CMD.c_str(), CIPSERVER_CMD.size()) < 0) {
        LOG_WARN("esp8266", "config write CIPSERVER failed");
    }
    sleep(DELAY_SEC);
#endif
    tcflush(fd, TCIOFLUSH);
    LOG_INFO("esp8266", "ESP8266 configured as AP mode");
    return 0;
}
