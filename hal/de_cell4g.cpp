#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include "hal/de_cell4g.h"
#include "infra/logger.h"
#include "infra/config.h"

/**
 * @brief 初始化4G模块，通过PPP拨号建立蜂窝网络连接
 *
 * 调用quectel-pppd.sh脚本启动PPP拨号，等待8秒后检测网络是否获取到IP地址。
 * 最多重试5次，若全部失败则调用kill脚本清理进程并返回错误。
 *
 * @param[in] net_name 网络接口名称，例如 "ppp0"
 *
 * @return 0  成功获取到IP地址
 * @return -1 5次重试后仍未成功连接
 */
int hard4g_init(char *net_name)
{
        int retry_count = 5;
        int success = 1;
        int sleep_sec = 8;  // 初始等待8秒，后续指数退避

        while (retry_count-- && success)
        {
                /* 启动PPP拨号脚本，后台运行连接到vnet.mobi网络 */
                std::string pppd_script = CFG_STR("network.cell4g", "pppd_script", "/home/root/g2020/mokuai/4g/ppp/ppp/quectel-pppd.sh");
                std::string pppd_cmd = pppd_script + " /dev/ttyUSB5 \"\" \"\" vnet.mobi &";
                system(pppd_cmd.c_str());
                /* PPP协商需要较长时间，等待拨号完成 */
                sleep(sleep_sec);
                if (cell4g_detect(net_name) == 0)
                        success = 0;
                else
                        sleep_sec = std::min(sleep_sec * 2, 32);  // 指数退避，最大32秒
        }

        if (success == 0)
                return 0;
        else
        {
                /* 所有重试失败，杀死PPP进程释放资源 */
                std::string pppd_kill = CFG_STR("network.cell4g", "pppd_kill", "/home/root/g2020/mokuai/4g/ppp/ppp/quectel-ppp-kill");
                system(pppd_kill.c_str());
                return -1;
        }
}

/**
 * @brief 检测4G网络是否已成功获取到IP地址
 *
 * 通过ioctl获取指定网络接口的IP地址，判断PPP拨号是否成功。
 * 调用前会先添加默认路由到ppp0接口。
 *
 * @param[in] net_name 要检测的网络接口名称，例如 "ppp0"
 *
 * @return  0 成功获取到有效IP地址
 * @return -1 获取IP失败（设备不存在或IP无效）
 */
int cell4g_detect(char *net_name)
{
        /* 校验 net_name 白名单：只允许字母、数字和点 */
        for (int i = 0; net_name[i] != '\0'; i++) {
                char c = net_name[i];
                if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                      (c >= '0' && c <= '9') || c == '.')) {
                        LOG_ERROR("cell4g", "Invalid net_name: contains disallowed character");
                        return -1;
                }
        }
        /* 添加默认路由，确保流量通过指定网络接口出口 */
        char route_cmd[128];
        snprintf(route_cmd, sizeof(route_cmd), "route add -net 0.0.0.0 netmask 0.0.0.0 dev %s", net_name);
        system(route_cmd);

        int sock_fd;
        struct sockaddr_in my_addr;
        struct ifreq ifr;
        char ipaddr[20];

        /* Get socket file descriptor */
        if ((sock_fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
        {
                LOG_ERROR("cell4g", "socket() failed: %s", strerror(errno));
                return -1;
        }

        /* Get IP Address */
        strncpy(ifr.ifr_name, net_name, IFNAMSIZ);
        ifr.ifr_name[IFNAMSIZ - 1] = '\0';

        if (ioctl(sock_fd, SIOCGIFADDR, &ifr) < 0)
        {
                LOG_ERROR("cell4g", "No such device: %s", net_name);
                close(sock_fd);
                return -1;
        }

        memcpy(&my_addr, &ifr.ifr_addr, sizeof(my_addr));
        strncpy(ipaddr, inet_ntoa(my_addr.sin_addr), sizeof(ipaddr) - 1);
        ipaddr[sizeof(ipaddr) - 1] = '\0';
        LOG_INFO("cell4g", "Network address: %s", ipaddr);
        if (strlen(ipaddr) == 0 || strcmp(ipaddr, "0.0.0.0") == 0){
                LOG_ERROR("cell4g", "IP is not valid");
                close(sock_fd);
                return -1;
        }

        close(sock_fd);
        return 0;
}

/**
 * @brief 通过TCP连接测试指定的服务器IP和端口是否可达
 *
 * 创建TCP套接字，尝试连接远程服务器。连接成功后发送一条登录消息。
 * 用于验证4G网络建立后能否正常访问目标服务器。
 *
 * @param[in] remote_ip   远程服务器的IP地址，例如 "106.52.84.156"
 * @param[in] remote_port 远程服务器的端口号，例如 8888
 *
 * @return >0  连接成功，返回套接字文件描述符
 * @return -1  连接失败
 */
int cell4g_ip_port_check(const char *remote_ip, int remote_port)
{
        int sockfd;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
                LOG_ERROR("cell4g", "Connect to server error");
                return -1;
        }
        else
        {
                struct sockaddr_in svraddr;
                memset(&svraddr, 0, sizeof(svraddr));
                svraddr.sin_family = AF_INET;
                svraddr.sin_port = htons(remote_port);
                if (inet_pton(AF_INET, remote_ip, &svraddr.sin_addr) <= 0) {
                    LOG_ERROR("cell4g", "inet_pton: invalid address %s", remote_ip);
                    close(sockfd);
                    return -1;
                }

                /* 设置连接超时15秒，避免阻塞导致看门狗误触发 */
                struct timeval tv_conn;
                tv_conn.tv_sec = 15;
                tv_conn.tv_usec = 0;
                setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv_conn, sizeof(tv_conn));
                int ret = connect(sockfd, (struct sockaddr *)&svraddr, sizeof(svraddr));
                if (ret < 0)
                {
                        LOG_ERROR("cell4g", "Connect to %s:%d failed", remote_ip, remote_port);
                        close(sockfd);
                        return -1;
                }
                else{
                    char buf[]="Device login\n";
                    if (write(sockfd, buf, sizeof(buf) - 1) < 0) {
                        LOG_ERROR("cell4g", "Write login message failed");
                        close(sockfd);
                        return -1;
                    }
                }
        }
        LOG_INFO("cell4g", "Connect server %s:%d success", remote_ip, remote_port);
        return sockfd;
}

/**
 * @brief 打开4G模块并连接到指定服务器
 *
 * 先初始化4G网络（PPP拨号获取IP），再通过TCP连接测试服务器。
 * 服务器地址硬编码为 106.52.84.156:8888。
 *
 * @return >0  连接成功，返回套接字文件描述符
 * @return -1  网络初始化失败或服务器连接失败
 */
int cell4g_open()
{
        if (hard4g_init("ppp0") == -1)
                return -1;

        std::string server_ip = CFG_STR("network.cell4g", "server_ip", "106.52.84.156");
        int server_port = CFG_INT("network.cell4g", "server_port", 8888);
        return cell4g_ip_port_check(server_ip.c_str(), server_port);
}
