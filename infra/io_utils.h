/**
 * @file io_utils.h
 * @brief I/O 工具函数
 * @details 提供通用的 I/O 操作函数
 */
#pragma once

#include <unistd.h>
#include <cstddef>

namespace sap {

/**
 * @brief 完整写入，处理 partial write
 * @param fd  文件描述符
 * @param buf 数据缓冲区
 * @param len 数据长度
 * @return 成功写入全部数据返回 0，失败返回 -1
 */
inline int write_full(int fd, const char* buf, size_t len)
{
    size_t total = 0;
    while (total < len) {
        ssize_t n = write(fd, buf + total, len - total);
        if (n == 0) return -2;
        if (n < 0)  return -1;
        total += n;
    }
    return 0;
}

} // namespace sap
