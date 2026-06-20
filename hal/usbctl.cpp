/**
 * @file usbctl.cpp
 * @brief 通用USB串口控制模块
 * @details 提供串口打开、配置、读写等基础操作
 *          - v1.0 2018/7/17 完成基本程序，支持最多9个串口
 *          - v2.0 2019/3/1 消除9个串口限制，最大支持100个端口
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include "hal/usbctl.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

/**
 * @brief 打开指定编号的USB串口设备（阻塞模式）
 * @param comport 串口编号：
 *        - 0: LoRa    /dev/tong_lora
 *        - 1: WiFi    /dev/tong_wifi
 *        - 2: 蓝牙    /dev/tong_bt
 *        - 3: 4G      /dev/tong_4g
 *        - 4: RS485(中) /dev/ttymxc2
 *        - 5: RS485(左) /dev/ttymxc4
 *        - 6: RS485(右) /dev/ttymxc1
 *        - 7: RS232   /dev/ttymxc3
 * @return 成功返回文件描述符fd(>0)，失败返回-1
 */
int open_port(int comport)
{
    int fd = -1;
    if (0 == comport)
    {
        fd = open("/dev/tong_lora", O_RDWR | O_NOCTTY | O_NDELAY);
        if (-1 == fd)
        {
            printf("Open lora fail!\n");
        }
    }
    else if (1 == comport)
    {
        fd = open("/dev/tong_wifi", O_RDWR | O_NOCTTY | O_NDELAY);
        if (-1 == fd)
        {
            printf("Open wifi fail!\n");
        }
    }
    else if (2 == comport)
    {
        fd = open("/dev/tong_bt", O_RDWR | O_NOCTTY | O_NDELAY);
        if (-1 == fd)
        {
            printf("Open bt fail!\n");
        }
    }
    else if (3 == comport)
    {
        fd = open("/dev/tong_4g", O_RDWR | O_NOCTTY | O_NDELAY);
        if (-1 == fd)
        {
            printf("Open 4g fail!\n");
        }
    }
    else if (4 == comport)
    {
        fd = open("/dev/ttymxc2", O_RDWR | O_NOCTTY | O_NDELAY);
        if (-1 == fd)
        {
            printf("Open ttymxc2 fail!\n");
        }
    }
    else if (5 == comport)
    {
        fd = open("/dev/ttymxc4", O_RDWR | O_NOCTTY | O_NDELAY);
        if (-1 == fd)
        {
            printf("Open ttymxc4 fail!\n");
        }
    }
    else if (6 == comport)
    {
        fd = open("/dev/ttymxc1", O_RDWR | O_NOCTTY | O_NDELAY);
        if (-1 == fd)
        {
            printf("Open ttymxc1 fail!\n");
        }
    }
    else if (7 == comport)
    {
        fd = open("/dev/ttymxc3", O_RDWR | O_NOCTTY | O_NDELAY);
        if (-1 == fd)
        {
            printf("Open ttymxc3 fail!\n");
        }
    }
    if (-1 == fd)
    {
        perror("Can't Open Serial Port");
        return (-1);
    }

    /* 恢复串口为阻塞模式 */
    if (fcntl(fd, F_SETFL, 0) < 0) {
        printf("fcntl failed!\n");
        close(fd);
        return -1;
    }
    return fd;
}

/**
 * @brief 配置串口参数（通用版本）
 * @param fd      文件描述符
 * @param nSpeed  波特率：2400/4800/9600/115200/460800，默认9600
 * @param nBits   数据位：7 或 8
 * @param nEvent  校验位：'o'奇校验 / 'e'偶校验 / 'n'无校验
 * @param nStop   停止位：1 或 2
 * @return 成功返回0，失败返回-1
 */
int set_opt1(int fd, int nSpeed, int nBits, uint8_t nEvent, int nStop)
{
    struct termios newtio, oldtio;
    if (tcgetattr(fd, &oldtio) != 0)
    {
        perror("SetupSerial 1");
        printf("tcgetattr( fd,&oldtio) -> %d\n", -1);
        return -1;
    }
    memset(&newtio, 0, sizeof(newtio));

    newtio.c_cflag |= (CLOCAL | CREAD);
    newtio.c_cflag &= ~CSIZE;
    switch (nBits)
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }
    switch (nEvent)
    {
    case 'o':
    case 'O':
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'e':
    case 'E':
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'n':
    case 'N':
        newtio.c_cflag &= ~PARENB;
        newtio.c_iflag &= ~INPCK;
        break;
    default:
        break;
    }
    switch (nSpeed)
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    case 460800:
        cfsetispeed(&newtio, B460800);
        cfsetospeed(&newtio, B460800);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }
    if (nStop == 1)
        newtio.c_cflag &= ~CSTOPB;
    else if (nStop == 2)
        newtio.c_cflag |= CSTOPB;

    /* VTIME/VMIN控制read()的阻塞行为，此处设为非阻塞 */
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    tcflush(fd, TCIFLUSH);
    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)
    {
        perror("com set error");
        return -1;
    }
    return 0;
}

/**
 * @brief 配置串口参数（支持硬件流控，主要用于RS485端口）
 * @param fd      文件描述符
 * @param nSpeed  波特率：2400/4800/9600/115200/460800，默认9600
 * @param nBits   数据位：7 或 8
 * @param nEvent  校验位：'o'奇校验 / 'e'偶校验 / 'n'无校验
 * @param nStop   停止位：1 或 2
 * @param flag    硬件流控：1开启CRTSCTS / 0关闭
 * @return 成功返回0，失败返回-1
 */
int set_opt(int fd, int nSpeed, int nBits, uint8_t nEvent, int nStop, int flag)
{
    struct termios newtio, oldtio;
    if (tcgetattr(fd, &oldtio) != 0)
    {
        perror("SetupSerial 1");
        printf("tcgetattr( fd,&oldtio) -> %d\n", -1);
        return -1;
    }
    memset(&newtio, 0, sizeof(newtio));

    newtio.c_cflag |= (CLOCAL | CREAD);
    newtio.c_cflag &= ~CSIZE;
    switch (flag)
    {
    case 0:
        break;
    case 1:
        newtio.c_cflag |= CRTSCTS;
        break;
    }
    switch (nBits)
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    default:
        break;
    }
    switch (nEvent)
    {
    case 'o':
    case 'O':
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'e':
    case 'E':
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'n':
    case 'N':
        newtio.c_cflag &= ~PARENB;
        newtio.c_iflag &= ~INPCK;
        break;
    default:
        break;
    }
    switch (nSpeed)
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    case 460800:
        cfsetispeed(&newtio, B460800);
        cfsetospeed(&newtio, B460800);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }
    if (nStop == 1)
        newtio.c_cflag &= ~CSTOPB;
    else if (nStop == 2)
        newtio.c_cflag |= CSTOPB;

    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    tcflush(fd, TCIFLUSH);
    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)
    {
        perror("com set error");
        return -1;
    }
    return 0;
}

/**
 * @brief 向串口写入数据
 * @param fd  文件描述符
 * @param buf 待发送数据的缓冲区首地址
 * @param len 待发送数据的字节长度
 * @return 成功返回已发送字节数，失败返回-1
 */
int write_port(int fd, const void* buf, size_t len)
{
    ssize_t ret;
    ret = write(fd, buf, len);
    if (ret < 0)
        return ret;
    return ret;
}

/**
 * @brief 从串口读取数据
 * @param fd  文件描述符
 * @param buf 接收数据的缓冲区首地址
 * @param len 期望读取的最大字节数，必须小于缓冲区大小
 * @return 成功返回已读取字节数，失败返回-1
 */
int read_port(int fd, void* buf, size_t len)
{
    ssize_t ret;
    ret = read(fd, buf, len);
    if (ret < 0)
        return ret;
    return ret;
}
