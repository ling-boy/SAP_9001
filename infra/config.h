/**
 * @file config.h
 * @brief 轻量级 INI 配置管理器（header-only）
 * @details 替代项目中 80+ 处硬编码配置值，支持 section.key 访问模式
 *          线程安全，支持配置文件热加载
 *
 * 使用示例：
 *   Config::instance().load("./config/device.conf");
 *   std::string ip = Config::instance().getString("network.wifi", "server_ip", "192.168.3.1");
 *   int port = Config::instance().getInt("network.wifi", "server_port", 1234);
 *   // 或使用点分格式：
 *   std::string ip2 = CFG_STR("network.wifi.server_ip", "192.168.3.1");
 *
 * @version 1.0
 */
#pragma once

#include <string>
#include <map>
#include <mutex>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>

namespace sap {

/**
 * @brief INI 格式配置管理器（单例模式）
 */
class Config {
public:
    /**
     * @brief 获取配置单例
     */
    static Config& instance() {
        static Config inst;
        return inst;
    }

    /**
     * @brief 加载 INI 配置文件
     * @param path 配置文件路径
     * @return 是否加载成功
     */
    bool load(const std::string& path) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::ifstream file(path);
        if (!file.is_open()) {
            return false;
        }

        data_.clear();
        std::string line;
        std::string currentSection;

        while (std::getline(file, line)) {
            // 去除行首尾空白
            trim(line);

            // 跳过空行和注释
            if (line.empty() || line[0] == '#') {
                continue;
            }

            // 解析 section [xxx.yyy]
            if (line[0] == '[') {
                size_t end = line.find(']');
                if (end != std::string::npos) {
                    currentSection = line.substr(1, end - 1);
                    trim(currentSection);
                }
                continue;
            }

            // 解析 key = value
            size_t eq = line.find('=');
            if (eq != std::string::npos) {
                std::string key = line.substr(0, eq);
                std::string value = line.substr(eq + 1);
                trim(key);
                trim(value);
                if (!currentSection.empty() && !key.empty()) {
                    data_[currentSection][key] = value;
                }
            }
        }

        filePath_ = path;
        return true;
    }

    /**
     * @brief 重新加载配置文件
     */
    bool reload() {
        if (filePath_.empty()) return false;
        return load(filePath_);
    }

    /**
     * @brief 获取字符串配置值
     * @param section    section 名称（如 "network.wifi"）
     * @param key        key 名称（如 "server_ip"）
     * @param defaultVal 默认值
     * @return 配置值，未找到则返回默认值
     */
    std::string getString(const std::string& section, const std::string& key,
                          const std::string& defaultVal = "") const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto secIt = data_.find(section);
        if (secIt != data_.end()) {
            auto keyIt = secIt->second.find(key);
            if (keyIt != secIt->second.end()) {
                return keyIt->second;
            }
        }
        return defaultVal;
    }

    /**
     * @brief 获取整数配置值
     */
    int getInt(const std::string& section, const std::string& key,
               int defaultVal = 0) const {
        std::string val = getString(section, key, "");
        if (val.empty()) return defaultVal;
        try {
            return std::stoi(val);
        } catch (...) {
            return defaultVal;
        }
    }

    /**
     * @brief 使用点分格式获取字符串配置值
     * @param dottedKey  点分格式 key（如 "network.wifi.server_ip"）
     * @param defaultVal 默认值
     * @note 最后一个 '.' 分隔 section 和 key
     */
    std::string getStringDotted(const std::string& dottedKey,
                          const std::string& defaultVal = "") const {
        std::string section, key;
        if (!splitDottedKey(dottedKey, section, key)) {
            return defaultVal;
        }
        return getString(section, key, defaultVal);
    }

    /**
     * @brief 使用点分格式获取整数配置值
     */
    int getIntDotted(const std::string& dottedKey, int defaultVal = 0) const {
        std::string section, key;
        if (!splitDottedKey(dottedKey, section, key)) {
            return defaultVal;
        }
        return getInt(section, key, defaultVal);
    }

    /**
     * @brief 检查 section 是否存在
     */
    bool hasSection(const std::string& section) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.find(section) != data_.end();
    }

    /**
     * @brief 检查 key 是否存在
     */
    bool hasKey(const std::string& section, const std::string& key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto secIt = data_.find(section);
        if (secIt != data_.end()) {
            return secIt->second.find(key) != secIt->second.end();
        }
        return false;
    }

    // 禁止拷贝
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

private:
    Config() = default;

    mutable std::mutex mutex_;
    std::map<std::string, std::map<std::string, std::string>> data_;
    std::string filePath_;

    /**
     * @brief 去除字符串首尾空白字符
     */
    static void trim(std::string& s) {
        // 去除首部空白
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
        }));
        // 去除尾部空白
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    /**
     * @brief 分割点分格式 key（最后一个 '.' 作为分隔符）
     * @return 是否成功分割
     */
    static bool splitDottedKey(const std::string& dottedKey,
                               std::string& section, std::string& key) {
        size_t lastDot = dottedKey.rfind('.');
        if (lastDot == std::string::npos || lastDot == 0) {
            return false;
        }
        section = dottedKey.substr(0, lastDot);
        key = dottedKey.substr(lastDot + 1);
        return !section.empty() && !key.empty();
    }
};

// ============================================================================
// 便捷宏
// ============================================================================

/**
 * @brief 使用 section + key 获取字符串配置
 * @example CFG_STR("network.wifi", "server_ip", "192.168.3.1")
 */
#define CFG_STR(section, key, ...) \
    sap::Config::instance().getString(section, key, ##__VA_ARGS__)

/**
 * @brief 使用 section + key 获取整数配置
 * @example CFG_INT("network.wifi", "server_port", 1234)
 */
#define CFG_INT(section, key, ...) \
    sap::Config::instance().getInt(section, key, ##__VA_ARGS__)

/**
 * @brief 使用点分格式获取字符串配置
 * @example CFG_STR_DOT("network.wifi.server_ip", "192.168.3.1")
 */
#define CFG_STR_DOT(dottedKey, ...) \
    sap::Config::instance().getStringDotted(dottedKey, ##__VA_ARGS__)

/**
 * @brief 使用点分格式获取整数配置
 * @example CFG_INT_DOT("network.wifi.server_port", 1234)
 */
#define CFG_INT_DOT(dottedKey, ...) \
    sap::Config::instance().getIntDotted(dottedKey, ##__VA_ARGS__)

} // namespace sap
