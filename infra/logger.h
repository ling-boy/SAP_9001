/**
 * @file logger.h
 * @brief 轻量级线程安全日志库（header-only）
 * @details 替代项目中 259 处 printf/cout 混用，提供统一的日志接口
 *          支持 6 级日志、模块标签、时间戳、线程ID、彩色输出
 *
 * 使用示例：
 *   LOG_INFO("sensor", "Temperature: %.2f C", temp);
 *   LOG_ERROR("comm", "Connection failed: %s", strerror(errno));
 *
 * @version 1.0
 */
#pragma once

#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <cstring>
#include <string>
#include <mutex>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>

namespace sap {

/**
 * @brief 日志级别枚举
 */
enum class LogLevel {
    TRACE,  ///< 最详细的追踪信息
    DEBUG,  ///< 调试信息，生产环境关闭
    INFO,   ///< 关键业务事件
    WARN,   ///< 可恢复的异常
    ERROR,  ///< 不可忽略的错误
    FATAL   ///< 致命错误，程序即将退出
};

/**
 * @brief 线程安全日志类（单例模式）
 */
class Logger {
public:
    /**
     * @brief 获取日志单例
     */
    static Logger& instance() {
        static Logger inst;
        return inst;
    }

    /**
     * @brief 设置最低日志级别（低于此级别的日志将被过滤）
     * @param level 日志级别
     */
    void setLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(mutex_);
        level_ = level;
    }

    /**
     * @brief 获取当前日志级别
     */
    LogLevel level() const { return level_; }

    /**
     * @brief 启用/禁用控制台输出
     */
    void enableConsole(bool enable) {
        std::lock_guard<std::mutex> lock(mutex_);
        console_ = enable;
    }

    /**
     * @brief 启用/禁用终端彩色输出
     */
    void enableColor(bool enable) {
        std::lock_guard<std::mutex> lock(mutex_);
        color_ = enable;
    }

    /**
     * @brief 设置日志输出文件
     * @param fp 文件指针（调用者负责关闭），传 nullptr 关闭文件输出
     */
    void setOutput(FILE* fp) {
        std::lock_guard<std::mutex> lock(mutex_);
        file_ = fp;
    }

    /**
     * @brief 设置日志文件路径（自动打开）
     * @param path 文件路径
     * @return 是否成功打开
     */
    bool setOutputFile(const std::string& path) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (file_ && file_ != stderr) {
            fclose(file_);
        }
        file_ = fopen(path.c_str(), "a");
        return file_ != nullptr;
    }

    /**
     * @brief 写入日志（printf 风格）
     * @param level   日志级别
     * @param module  模块标签（如 "sensor", "comm", "main"）
     * @param fmt     格式化字符串
     * @param ...     可变参数
     */
    void log(LogLevel level, const char* module, const char* fmt, ...) {
        if (level < level_) return;

        // 格式化用户消息
        char msg[512];
        va_list args;
        va_start(args, fmt);
        vsnprintf(msg, sizeof(msg), fmt, args);
        va_end(args);

        // 获取时间戳（含毫秒）
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        struct tm tm_info;
        localtime_r(&tv.tv_sec, &tm_info);

        char timestamp[40];
        snprintf(timestamp, sizeof(timestamp), "%04d-%02d-%02d %02d:%02d:%02d.%03d",
                 tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday,
                 tm_info.tm_hour, tm_info.tm_min, tm_info.tm_sec,
                 (int)(tv.tv_usec / 1000));

        // 获取线程 ID
        pid_t tid = syscall(SYS_gettid);

        // 拼接完整日志行
        char line[1024];
        snprintf(line, sizeof(line), "[%s] [%-5s] [%-8s] [T:%d] %s",
                 timestamp, levelString(level), module, tid, msg);

        std::lock_guard<std::mutex> lock(mutex_);

        // 控制台输出
        if (console_) {
            if (color_ && isatty(STDERR_FILENO)) {
                fprintf(stderr, "%s%s%s\n", levelColor(level), line, COLOR_RESET);
            } else {
                fprintf(stderr, "%s\n", line);
            }
        }

        // 文件输出
        if (file_) {
            fprintf(file_, "%s\n", line);
            fflush(file_);
        }
    }

    // 禁止拷贝
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    LogLevel level_ = LogLevel::INFO;
    bool console_ = true;
    bool color_ = true;
    FILE* file_ = nullptr;
    std::mutex mutex_;

    // ANSI 颜色码
    static constexpr const char* COLOR_TRACE = "\033[90m";    // 灰色
    static constexpr const char* COLOR_DEBUG = "\033[36m";    // 青色
    static constexpr const char* COLOR_INFO  = "\033[32m";    // 绿色
    static constexpr const char* COLOR_WARN  = "\033[33m";    // 黄色
    static constexpr const char* COLOR_ERROR = "\033[31m";    // 红色
    static constexpr const char* COLOR_FATAL = "\033[35m";    // 紫色
    static constexpr const char* COLOR_RESET = "\033[0m";

    Logger() = default;
    ~Logger() {
        if (file_ && file_ != stderr) {
            fclose(file_);
        }
    }

    static const char* levelString(LogLevel level) {
        switch (level) {
            case LogLevel::TRACE: return "TRACE";
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO:  return "INFO ";
            case LogLevel::WARN:  return "WARN ";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::FATAL: return "FATAL";
            default:              return "?????";
        }
    }

    static const char* levelColor(LogLevel level) {
        switch (level) {
            case LogLevel::TRACE: return COLOR_TRACE;
            case LogLevel::DEBUG: return COLOR_DEBUG;
            case LogLevel::INFO:  return COLOR_INFO;
            case LogLevel::WARN:  return COLOR_WARN;
            case LogLevel::ERROR: return COLOR_ERROR;
            case LogLevel::FATAL: return COLOR_FATAL;
            default:              return COLOR_RESET;
        }
    }
};

} // namespace sap

// ============================================================================
// 便捷宏：自动填充模块名
// 注意：syslog.h 定义了 LOG_INFO/LOG_ERR 等宏，需要先 undef 避免冲突
// ============================================================================

#ifdef LOG_TRACE
#undef LOG_TRACE
#endif
#ifdef LOG_DEBUG
#undef LOG_DEBUG
#endif
#ifdef LOG_INFO
#undef LOG_INFO
#endif
#ifdef LOG_WARNING
#undef LOG_WARNING
#endif
#ifdef LOG_WARN
#undef LOG_WARN
#endif
#ifdef LOG_ERR
#undef LOG_ERR
#endif
#ifdef LOG_ERROR
#undef LOG_ERROR
#endif
#ifdef LOG_FATAL
#undef LOG_FATAL
#endif

// Force undef any existing definitions from syslog.h
#undef LOG_TRACE
#undef LOG_DEBUG
#undef LOG_INFO
#undef LOG_WARN
#undef LOG_ERR
#undef LOG_ERROR
#undef LOG_FATAL

#define LOG_TRACE(module, fmt, ...) sap::Logger::instance().log(sap::LogLevel::TRACE, module, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(module, fmt, ...) sap::Logger::instance().log(sap::LogLevel::DEBUG, module, fmt, ##__VA_ARGS__)
#define LOG_INFO(module,  fmt, ...) sap::Logger::instance().log(sap::LogLevel::INFO,  module, fmt, ##__VA_ARGS__)
#define LOG_WARN(module,  fmt, ...) sap::Logger::instance().log(sap::LogLevel::WARN,  module, fmt, ##__VA_ARGS__)
#define LOG_ERROR(module, fmt, ...) sap::Logger::instance().log(sap::LogLevel::ERROR, module, fmt, ##__VA_ARGS__)
#define LOG_FATAL(module, fmt, ...) sap::Logger::instance().log(sap::LogLevel::FATAL, module, fmt, ##__VA_ARGS__)
