#include "sensor/usart.h"
#include "protocol/protocol_process.h"
using namespace std;

/**
 * @brief 监听RS485和RS232串口数据，收到数据后封装为06包推送到队列
 * @param arg 指向monitor485结构体的指针，包含消息队列等资源
 * @return 线程退出时返回nullptr
 * @details 打开3个串口（RS485_A、RS485_C、RS232），使用select()多路复用监听，
 *          收到数据后加"##"前缀标识来源，再通过packet06()封装为私有协议06包推入队列
 */
void* RS485_Monitor(void *arg){
    cout<<"Enabling Serial Port Listening!"<<endl;
    char buf[1024];
    struct monitor485 *monitor485 = (struct monitor485 *)arg;
    const char* threadname = "RS485_Monitor";
    int wdt_id = monitor485->g_CsoftwareWdt->RequestSoftwareWdtID(threadname, 120);
    int fdRs485One = open_port(5);
    cout<<"fdRs485One:"<<fdRs485One<<endl;
    int fdRs485Two = open_port(6); 
    cout<<"fdRs485Two:"<<fdRs485Two<<endl;
    int fdRs232 = open_port(7);
    cout<<"fdRs232:"<<fdRs232<<endl;
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
            fprintf(stderr, "set_opt RS485_A failed\n");
            close(fdRs485One);
            close(fdRs485Two);
            close(fdRs232);
            return nullptr;
        }
        if (set_opt(fdRs485Two, 9600, 8, 'n', 1, 1) < 0) {
            fprintf(stderr, "set_opt RS485_C failed\n");
            close(fdRs485One);
            close(fdRs485Two);
            close(fdRs232);
            return nullptr;
        }
        if (set_opt(fdRs232, 9600, 8, 'n', 1, 1) < 0) {
            fprintf(stderr, "set_opt RS232 failed\n");
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
    cout<<"File Descriptor-1："<<fdRs485One<<endl;
    cout<<"File Descriptor-2："<<fdRs485Two<<endl;
    cout<<"File Descriptor-3："<<fdRs232<<endl;
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
            perror("select error in RS485_Monitor");
            continue;
        }
        if(ret == 0){
            /* select超时，喂狗后继续循环 */
            continue;
        }
        if(ret>0){
            cout<<"Serial Port Has Data"<<endl;
            if (FD_ISSET(fdRs485One, &fdset)){
                ret = read(fdRs485One, buf, 1024);
                if(ret == 0){
                    cout<<"RS485One: device disconnected"<<endl;
                    break;
                }
                if(ret < 0){
                    perror("read RS485One");
                    continue;
                }
                if(ret>0){
                    string str(buf, ret);
                    str = "##" + str; /* 收到数据后加"##"前缀标识数据来源 */
                    string strr = packet06(str, RS485_A); /* 封装为私有协议06包，标记来源为RS485_A */
                    monitor485->que->push(strr);
                    cout<<"The Port RS485 Data is："<<str<<endl;
                }
                memset(buf,0,sizeof(buf));
            }
            if (FD_ISSET(fdRs485Two, &fdset)){
                ret = read(fdRs485Two, buf, 1024);
                if(ret == 0){
                    cout<<"RS485Two: device disconnected"<<endl;
                    break;
                }
                if(ret < 0){
                    perror("read RS485Two");
                    continue;
                }
                if(ret>0){
                    string str(buf, ret);
                    str = "##" + str; /* 收到数据后加"##"前缀标识数据来源 */
                    string strr = packet06(str, RS485_C); /* 封装为私有协议06包，标记来源为RS485_C */
                    monitor485->que->push(strr);
                    cout<<"The Port RS485 Data is："<<str<<endl;
                }
                memset(buf,0,sizeof(buf));
            }
            if (FD_ISSET(fdRs232, &fdset)){
                ret = read(fdRs232, buf, 1024);
                if(ret == 0){
                    cout<<"RS232: device disconnected"<<endl;
                    break;
                }
                if(ret < 0){
                    perror("read RS232");
                    continue;
                }
                if(ret>0){
                    string str(buf, ret);
                    str = "##" + str; /* 收到数据后加"##"前缀标识数据来源 */
                    string strr = packet06(str, RS232); /* 封装为私有协议06包，标记来源为RS232 */
                    monitor485->que->push(strr);
                    cout<<"The Port RS232 Data is："<<str<<endl;
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