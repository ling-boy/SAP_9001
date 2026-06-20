#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>
#include <string>
#include <iostream>
#include "hal/lora.h"
#include "hal/usbctl.h"
using namespace std;
/**
 * @brief 打开LoRa串口设备并配置通信参数
 *
 * 打开端口号为0的串口设备（LoRa模块），配置波特率115200、8数据位、无校验、1停止位。
 *
 * @return >0  成功，返回串口文件描述符
 * @return -1  打开串口失败
 */
int lora_open()
{
    int lora;
    lora = open_port(0);
    if(-1 == lora){
        return -1;
    }
    else{
        if (set_opt1(lora, 115200, 8, 'n', 1) < 0) {
            close(lora);
            return -1;
        }
    }
    return lora;
}
