#include "app/regist_manage.h"
#include "hal/gps.h"
#include <sys/time.h>
#include <time.h>

/**
 * @brief 完整写入，处理 partial write
 * @return 成功写入全部数据返回 0，失败返回 -1
 */
static int write_full(int fd, const char* buf, size_t len)
{
    size_t total = 0;
    while (total < len) {
        ssize_t n = write(fd, buf + total, len - total);
        if (n <= 0) return -1;
        total += n;
    }
    return 0;
}

/**
 * @brief 设备注册线程，通过select监听通信设备，发送注册请求，等待ISR下发17协议参数和20协议时间戳
 * @param arg 设备注册结构体指针（deviceRegist），包含软件看门狗、通信设备向量等资源
 * @return 成功返回(void*)0，失败返回(void*)-1
 * @note 注册流程：监听信道 -> 发送01注册请求 -> 等待17协议参数 -> 发送02确认 -> 等待20协议时间戳 -> 设置系统时间 -> 使能设备
 */
void* device_regist(void* arg)
{
    cout << "device_regist start!!!" << endl;
    struct timeval tv1;
    struct timeval tv2;
    uint8_t RX_buf[RX_SIZE];
    int myCount = 0;
    int ret;
    int select_num = 1;
    int regist_num = 1;
    fd_set fdset;
    FD_ZERO(&fdset);
    struct deviceRegist* dev = (struct deviceRegist*)arg;
    CSoftwareWdt* g_CsoftwareWdt = dev->g_CsoftwareWdt;
    vector<vector<int>>* vec = dev->vec;
    if ((*vec).empty()) {
        cout << "device_regist: no communication devices available" << endl;
        return (void*)-1;
    }
    int size = static_cast<int>((*vec).size()) - 1;
    const char* const threadname = "device_regist";
    int wdt_id = -1;
    const int softdogTimeout = 60;
    wdt_id = g_CsoftwareWdt->RequestSoftwareWdtID(threadname, softdogTimeout);
    string regist_message;
    int i;
    cout << " device_regist 1" << endl;
    g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
    cout << " device_regist 1" << endl;
/* 注册失败重试入口：跳转至此重新选择信道进行注册 */
select_again:
    /* 当前通信设备已重试5次，切换到下一个通信设备 */
    if (regist_num == 5)
    {
        size--;
        regist_num = 1;
        cout << "下一个通信设备开始注册" << endl;
        sleep(5);
    }
    /* 一轮注册完毕（所有通信设备均尝试失败），退出注册线程 */
    if (size < 0)
    {
        size = (*vec).size() - 1;
        /* 硬件清理：关闭LED、关闭12V电源、同步RTC */
        system("echo 0 > /sys/class/leds/red/brightness");
        system("echo 0 > /sys/class/leds/yellow/brightness");
        system("echo 0 > /sys/class/leds/green/brightness");
        system("/home/root/power_12v.sh off");
        system("hwclock -w");
        system("killall wpa_supplicant");
        sleep(1);
        system("killall udhcpc");
        cout << "一轮已经注册完毕, 退出注册线程" << endl;
        return (void*)-1;

    }
    while (1)
    {
        FD_ZERO(&fdset);
        FD_SET((*vec)[size][1], &fdset);
        tv1.tv_sec = 15;
        tv1.tv_usec = 0;
        myCount++;
        if (myCount == 1000) return (void*)-1;
        cout << " device_regist 8" << endl;
        g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
        cout << " device_regist 8" << endl;
        ret = select((*vec)[size][1] + 1, &fdset, NULL, NULL, &tv1);
        cout << " device_regist 8 ----- 8" << endl;
        if (ret > 0)
        {
            if (FD_ISSET((*vec)[size][1], &fdset))
            {
                memset(RX_buf, 0, sizeof(RX_buf));
                ret = read((*vec)[size][1], RX_buf, RX_SIZE);
                if (ret <= 0) {
                    cout << "regist read error or EOF, ret=" << ret << endl;
                    continue;
                }
                if (ret >= 2 && ret < 58 && RX_buf[0] != '$' && RX_buf[ret - 1] == '@') //该包为最后一个数据包
                {
                    cout << " device_regist 2" << endl;
                    g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
                    cout << " device_regist 2" << endl;
                    goto sendreg_entrance;
                }
                else
                {
                    if (RX_buf[0] == '$' && RX_buf[ret - 1] == '@' && select_num == 5)
                    {
                        cout << " device_regist 3" << endl;
                        g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
                        cout << " device_regist 3" << endl;
                        goto sendreg_entrance;
                    }
                    else
                    {
                        select_num++;
                    }
                }
            }
        }
        /* select超时（15秒），表示信道空闲，可以发送注册请求 */
        else if (ret == 0)
        {
            cout << " device_regist 4" << endl;
            g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
            cout << " device_regist 4" << endl;
            goto sendreg_entrance;
        }
        else {
            return (void*)-1;
        }
    }
/* 发送注册请求入口：组装01协议注册包并发送 */
sendreg_entrance:
    sleep(monitor_time / 4);
    regist_message = string("$") + "01" + to_string((*vec)[size][0]) + id + net_id + "00" + "0026" + mac + communicate_status + "010" + gps_get_location() + cpu_occupy + "@";
    ret = write_full((*vec)[size][1], regist_message.c_str(), regist_message.size());
    if (ret < 0)
    {
        cout << "device_regist: write error:" << strerror(errno) << endl;
        select_num = 1;
        regist_num++;
        goto select_again;
    }
    else
    {
        cout << "ret:" << ret << endl;
        cout << "Regist: " << regist_message << endl;
        i = 5;
        while (i-- > 0)
        {
            FD_ZERO(&fdset);
            FD_SET((*vec)[size][1], &fdset);
            tv2.tv_sec = monitor_time;
            tv2.tv_usec = 0;
            cout << " device_regist 5" << endl;
            g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
            cout << " device_regist 5" << endl;
            ret = select((*vec)[size][1] + 1, &fdset, NULL, NULL, &tv2);
            cout << "ret >> " << ret << endl;
            if (ret > 0)
            {
                cout << " device_regist 6" << endl;
                g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
                cout << " device_regist 6" << endl;
                memset(RX_buf, 0, sizeof(RX_buf));
                ret = read((*vec)[size][1], RX_buf, RX_SIZE);
                if (ret <= 0) {
                    cout << "regist read error or EOF, ret=" << ret << endl;
                    continue;
                }
#ifdef DEBUG
                cout << "Regist recv：" << RX_buf << endl;
#endif
                string regist_recv_message(RX_buf, RX_buf + ret);
                if (regist_recv_message.length() < 54) {
                    cout << "regist_recv_message too short: " << regist_recv_message.length() << endl;
                    continue;
                }
                string recv_protocal = regist_recv_message.substr(1, 2);
                /* 收到17协议，提取ISR的MAC地址、设备ID和网络ID */
                if (recv_protocal == REQ_SEND_INFO)
                {
                    string recv_mac = regist_recv_message.substr(16, 16);
                    if (recv_mac == mac)
                    {
                        Isr_mac = regist_recv_message.substr(32, 16);
                        id = regist_recv_message.substr(48, 2);
                        cout << "网关下发的ID：" << id << endl;
                        net_id = regist_recv_message.substr(50, 4);
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }
                else
                {
                    continue;
                }
            }
        }
        if (id != "FF")
        {
            int listen_num = 8;
            /* 发送02确认包，通知ISR注册参数已收到 */
            string confirm_message = "";
            confirm_message = confirm_message + "$" + "02" + to_string((*vec)[size][0]) + id + net_id + "00" + "0012" + mac + id + "@";
            ret = write_full((*vec)[size][1], confirm_message.c_str(), confirm_message.size());
            if (ret < 0)
            {
                printf("write confirm_message failed!");
            }
            cout << "confirm_message:" << confirm_message << endl;
            /* 发送02确认包后，等待ISR下发20协议时间戳 */
            cout << "等待下发时间戳" << endl;
            while (listen_num)
            {
                cout << " device_regist 7" << endl;
                g_CsoftwareWdt->KeepSoftwareWdtAlive(wdt_id);
                cout << " device_regist 7" << endl;
                FD_ZERO(&fdset);
                FD_SET((*vec)[size][1], &fdset);
                tv2.tv_sec = monitor_time;
                tv2.tv_usec = 0;
                ret = select((*vec)[size][1] + 1, &fdset, NULL, NULL, &tv2);
                listen_num--;
                cout << "时间戳监听计数器：" << listen_num << endl;
                if (ret > 0)
                {
                    memset(RX_buf, 0, sizeof(RX_buf));
                    ret = read((*vec)[size][1], RX_buf, RX_SIZE);
                    if (ret <= 0) {
                        cout << "ISR QUIT or read error, ret=" << ret << endl;
                        continue;
                    }
                    string time_recvmessage(RX_buf, RX_buf + ret);
                    cout << time_recvmessage << endl;
                    if (time_recvmessage.length() < 49) {
                        cout << "time_recvmessage too short: " << time_recvmessage.length() << endl;
                        continue;
                    }
                    string recv_protocal = time_recvmessage.substr(1, 2);
                    cout << recv_protocal << endl;
                    string recv_mac = time_recvmessage.substr(16, 16);
                    cout << recv_mac << endl;
                    /* 收到20协议时间戳，设置系统时间并使能设备 */
                    if (recv_protocal == TIME_SEND && recv_mac == mac)
                    {
                        current_time = time_recvmessage.substr(32, 17);   //获取网关下发参数时间戳 2016 08 01 08 58 57 223
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
                            cout << "mktime failed, invalid timestamp" << endl;
                            continue;
                        }
                        tv_set.tv_usec = atoi(current_time.substr(14, 3).c_str()) * 1000;
                        if (settimeofday(&tv_set, NULL) != 0) {
                            cout << "settimeofday failed: " << strerror(errno) << endl;
                        }
                        cout << "设置系统时间成功: " << current_time << endl;
                        /* 设置系统时间后，将当前通信设备设置为使能状态，注册成功 */
                        if (CM->setEnable((*vec)[size][0]) == true)   //将其设置为注册成功使能状态
                        {
                            cout << "ID号为：" << (*vec)[size][0] << "使能" << endl;
                            cout << "SAP注册成功" << endl;
                            g_CsoftwareWdt->ReleaseSoftwareWdtID(threadname, wdt_id); //解除对此线程的监控
                            return (void*)0;
                        }
                        break;
                    }
                }
            }
            /* 等待时间戳超时（监听计数耗尽），此次注册失败 */
            if (listen_num == 0) {
                /* 硬件清理：关闭LED、关闭12V电源、同步RTC */
                system("echo 0 > /sys/class/leds/red/brightness");
                system("echo 0 > /sys/class/leds/yellow/brightness");
                system("echo 0 > /sys/class/leds/green/brightness");
                system("/home/root/power_12v.sh off");
                system("hwclock -w");
                system("killall wpa_supplicant");
                sleep(1);
                system("killall udhcpc");
                return (void*)-1; //此次未注册成功，重新开始注册
            }
        }
        /* 收到的17协议中ID为"FF"，表示注册被拒绝，重试 */
        else
        {
            select_num = 1;
            regist_num++;
            goto select_again;
        }
    }
    return (void*)-1;
}

