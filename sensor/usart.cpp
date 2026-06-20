#include "sensor/usart.h"
#include "protocol/protocol_process.h"
#include "infra/logger.h"
#include <cerrno>

/**
 * @brief 监听RS485和RS232串口数据，收到数据后封装为06包推送到队列
 * @param arg 指向monitor485结构体的指针，包含消息队列等资源
 * @return 线程退出时返回nullptr
 * @details 打开3个串口（RS485_A、RS485_C、RS232），使用select()多路复用监听，
 *          收到数据后加"##"前缀标识来源，再通过packet06()封装为私有协议06包推入队列
 */
void* RS485_Monitor(void *arg){
    LOG_INFO("usart", "Enabling Serial Port Listening!");
    char buf[1024];
    struct monitor485 *monitor485 = (struct monitor485 *)arg;
    const char* threadname = "RS485_Monitor";
    int wdt_id = monitor485->g_CsoftwareWdt->RequestSoftwareWdtID(threadname, 120);
    int fdRs485One = open_port(5);
    LOG_INFO("usart", "fdRs485One:%d", fdRs485One);
    int fdRs485Two = open_port(6);
    LOG_INFO("usart", "fdRs485Two:%d", fdRs485Two);
    int fdRs232 = open_port(7);
    LOG_INFO("usart", "fdRs232:%d", fdRs232);
    int maxNum = fdRs485One;
    if (fdRs485Two > maxNum) maxNum = fdRs485Two;
    if (fdRs232 > maxNum) maxNum = fdRs232;
    if(fdRs485One == -1 || fdRs485Two == -1 || fdRs232 == -1){
        if (fdRs485One != -1) close(fdRs485One);
        if (fdRs485Two != -1) close(fdRs485Two);
        if (fdRs232 != -1) close(fdRs232);
        return nullptr;
    }else{
        if (set_opt(fdRs485One, 9600, 8, 'n', 1, 1) < 0) {
            LOG_ERROR("usart", "set_opt RS485_A failed");
            close(fdRs485One);
            close(fdRs485Two);
            close(fdRs232);
            return nullptr;
        }
        if (set_opt(fdRs485Two, 9600, 8, 'n', 1, 1) < 0) {
            LOG_ERROR("usart", "set_opt RS485_C failed");
            close(fdRs485One);
            close(fdRs485Two);
            close(fdRs232);
            return nullptr;
        }
        if (set_opt(fdRs232, 9600, 8, 'n', 1, 1) < 0) {
            LOG_ERROR("usart", "set_opt RS232 failed");
            close(fdRs485One);
            close(fdRs485Two);
            close(fdRs232);
            return nullptr;
        }
    }
    struct timeval timeout;
    timeout.tv_sec = 60;
    timeout.tv_usec = 0;
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(fdRs485One,&fdset);
    FD_SET(fdRs485Two,&fdset);
    FD_SET(fdRs232,&fdset);
    LOG_INFO("usart", "File Descriptor-1：%d", fdRs485One);
    LOG_INFO("usart", "File Descriptor-2：%d", fdRs485Two);
    LOG_INFO("usart", "File Descriptor-3：%d", fdRs232);
    int ret;
    while(1){
        monitor485->g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
        FD_ZERO(&fdset);
        FD_SET(fdRs485One, &fdset);
        FD_SET(fdRs485Two, &fdset);
        FD_SET(fdRs232, &fdset);
        /* select()监听多个串口文件描述符，超时60秒后喂狗继续 */
        timeout.tv_sec = 60;
        timeout.tv_usec = 0;
        ret = select(maxNum + 1, &fdset, NULL, NULL, &timeout);
        if(ret < 0){
            LOG_ERROR("usart", "select error in RS485_Monitor: %s", strerror(errno));
            continue;
        }
        if(ret == 0){
            /* select超时，喂狗后继续循环 */
            continue;
        }
        if(ret>0){
            LOG_INFO("usart", "Serial Port Has Data");
            if (FD_ISSET(fdRs485One, &fdset)){
                ret = read(fdRs485One, buf, 1024);
                if(ret == 0){
                    LOG_WARN("usart", "RS485One: device disconnected");
                    break;
                }
                if(ret < 0){
                    LOG_ERROR("usart", "read RS485One: %s", strerror(errno));
                    continue;
                }
                if(ret>0){
                    std::string str(buf, ret);
                    str = "##" + str; /* 收到数据后加"##"前缀标识数据来源 */
                    std::string strr = packet06(str, RS485_A); /* 封装为私有协议06包，标记来源为RS485_A */
                    monitor485->que->push(strr);
                    LOG_INFO("usart", "The Port RS485 Data is：%s", str.c_str());
                }
                memset(buf,0,sizeof(buf));
            }
            if (FD_ISSET(fdRs485Two, &fdset)){
                ret = read(fdRs485Two, buf, 1024);
                if(ret == 0){
                    LOG_WARN("usart", "RS485Two: device disconnected");
                    break;
                }
                if(ret < 0){
                    LOG_ERROR("usart", "read RS485Two: %s", strerror(errno));
                    continue;
                }
                if(ret>0){
                    std::string str(buf, ret);
                    str = "##" + str; /* 收到数据后加"##"前缀标识数据来源 */
                    std::string strr = packet06(str, RS485_C); /* 封装为私有协议06包，标记来源为RS485_C */
                    monitor485->que->push(strr);
                    LOG_INFO("usart", "The Port RS485 Data is：%s", str.c_str());
                }
                memset(buf,0,sizeof(buf));
            }
            if (FD_ISSET(fdRs232, &fdset)){
                ret = read(fdRs232, buf, 1024);
                if(ret == 0){
                    LOG_WARN("usart", "RS232: device disconnected");
                    break;
                }
                if(ret < 0){
                    LOG_ERROR("usart", "read RS232: %s", strerror(errno));
                    continue;
                }
                if(ret>0){
                    std::string str(buf, ret);
                    str = "##" + str; /* 收到数据后加"##"前缀标识数据来源 */
                    std::string strr = packet06(str, RS232); /* 封装为私有协议06包，标记来源为RS232 */
                    monitor485->que->push(strr);
                    LOG_INFO("usart", "The Port RS232 Data is：%s", str.c_str());
                }
                memset(buf,0,sizeof(buf));
            }
        }
    }
    close(fdRs485One);
    close(fdRs485Two);
    close(fdRs232);
    return nullptr;
}
