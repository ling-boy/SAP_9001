#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>
#include <string>
#include "hal/bluetooth.h"
#include "hal/usbctl.h"
#include "infra/logger.h"
#include "infra/config.h"


/**
 * @brief 解析蓝牙扫描返回的一行消息，判断是否找到目标设备
 *
 * @param[in,out] meg 输入为蓝牙模块返回的一行原始数据；若找到g2020设备，
 *                    输出会被截取为前12字符的MAC地址
 *
 * @return 1 未找到目标设备（空消息或非g2020设备）
 * @return 2 找到g2020设备，meg中已包含MAC地址
 * @return 3 扫描结束（收到END标记）
 */
int blue_proc_message(std::string &meg)
{
    if(meg.empty())
    {
        return 1;
    }
    /* 检查是否收到扫描结束标记 */
    if(strstr(meg.c_str(), "END") != NULL)
    {
        return 3;
    }
    /* 找到g2020设备，截取前12位作为MAC地址 */
    else if(strstr(meg.c_str(), "g2020") != NULL)
    {
        if (meg.length() >= 12)
            meg = meg.substr(0, 12);
        return 2;
    }
    /* 非目标设备，跳过 */
    else
    {
        return 1;
    }
}
/**
 * @brief 打开蓝牙模块，扫描附近设备并查找g2020设备
 *
 * 通过串口向蓝牙模块发送AT指令，执行设备扫描。扫描结果逐行解析，
 * 若找到名为g2020的设备则将其MAC地址写入device_mac并返回串口fd。
 *
 * @param[out] device_mac 找到g2020设备时，输出其12位MAC地址字符串
 *
 * @return >0  找到g2020设备，返回串口文件描述符（调用方负责关闭）
 * @return -1  未找到设备或串口打开失败
 */
int bluetooth_open(std::string &device_mac)
{
    int fd = -1, ret;
    char buf[61];
    char in;
    uint8_t RST[] = "AT+Z=1\r\n";          /* AT+Z=1: 重启蓝牙模块 */
    uint8_t OPEN_VISUAL[] = "AT+SPP=1\r\n"; /* AT+SPP=1: 设置蓝牙可见，允许被扫描 */
    uint8_t AT_SCAN[] = "AT+SCAN?\r\n";     /* AT+SCAN?: 启动蓝牙设备扫描 */
    uint8_t DISCONNECT[] = "AT+DISCON=1\r\n"; /* AT+DISCON=1: 断开当前蓝牙连接 */
    std::string mac;
    std::string pro_message = "";
    struct timeval timeout;
    fd_set fdset;
    FD_ZERO(&fdset);

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    int i = 5;
    while(i > 0)
    {
        fd = open_port(2);
        if (-1 == fd)
        {
            i--;
            continue;
        }
        else{
            break;
        }
    }
    if(fd != -1)
    {
        ret = set_opt1(fd, 115200, 8, 'n', 1);
        if (ret < 0) {
            LOG_ERROR("bluetooth", "set_opt1 failed");
            close(fd);
            return -1;
        }
        FD_SET(fd,&fdset);
        /* jd_open.sh: 拉低switch管脚，蓝牙模块进入AT命令模式 */
        std::string bt_open = CFG_STR("paths", "bt_open_script", "/home/root/jd_open.sh");
        system(bt_open.c_str());
        sleep(1);
        if (write_port(fd, DISCONNECT, strlen((const char*)DISCONNECT)) < 0) {
            LOG_WARN("bluetooth", "write DISCONNECT failed");
        }
        /* jd_close.sh: 拉高switch管脚，蓝牙模块退出AT命令模式 */
        std::string bt_close = CFG_STR("paths", "bt_close_script", "/home/root/jd_close.sh");
        system(bt_close.c_str());
        /* 发送重启指令，等待模块复位完成 */
        if (write_port(fd, (const char*)RST, strlen((const char*)RST)) < 0) {
            LOG_WARN("bluetooth", "write RST failed");
        }
        sleep(1);
        ret = select(fd + 1, &fdset, NULL, NULL, &timeout);
        switch (ret)
        {
        case -1:
            LOG_ERROR("bluetooth", "select() error");
            break;
        case 0:
            LOG_WARN("bluetooth", "select() timeout");
            break;
        default:
            if (FD_ISSET(fd, &fdset))
            {
                ret = read_port(fd, buf, 60);
                if (ret > 0)
                {
                    buf[ret] = '\0';
                     // 检查AT指令返回值
                    char *p = strstr(buf, "OK");
                    if (p != NULL)
                    {
                        //设备重启成功
                        /* 设置蓝牙可见 */
                        ret = write_port(fd, OPEN_VISUAL, strlen((const char*)OPEN_VISUAL));
                        if (ret < 0) {
                            LOG_ERROR("bluetooth", "write OPEN_VISUAL failed");
                            close(fd);
                            return -1;
                        }
                        sleep(1);
                        /* 开始扫描附近蓝牙设备 */
                        ret = write_port(fd, AT_SCAN, strlen((const char*)AT_SCAN));
                        if (ret < 0) {
                            LOG_ERROR("bluetooth", "write AT_SCAN failed");
                            close(fd);
                            return -1;
                        }
                        int retry_count = 30;
                        while(retry_count > 0)
                        {
                            FD_ZERO(&fdset);
                            FD_SET(fd, &fdset);
                            struct timeval scan_timeout;
                            scan_timeout.tv_sec = 30;
                            scan_timeout.tv_usec = 0;
                            ret = select(fd + 1, &fdset, NULL, NULL, &scan_timeout);
                            if(ret <= 0)
                            {
                                retry_count--;
                                continue;
                            }
                            if(ret > 0)
                            {
                                ssize_t n = read(fd, &in, 1);
                                if(n <= 0) break;
                                if(in != '\n')
                                {
                                   pro_message = pro_message + in;
                                }
                                else
                                {
                                    int ret_proc_mess = blue_proc_message(pro_message);
                                    if(ret_proc_mess == 1)
                                    {
                                        pro_message = "";
                                        continue;
                                    }
                                    else if(ret_proc_mess == 2)
                                    {
                                        device_mac = pro_message;
                                        LOG_INFO("bluetooth", "g2020 found: %s", device_mac.c_str());
                                        return fd;
                                    }
                                    else if(ret_proc_mess == 3)
                                    {
                                        FD_CLR(fd, &fdset);
                                        close(fd);
                                        return -1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (fd >= 0) {
        FD_CLR(fd, &fdset);
        close(fd);
    }
    return -1;
}
