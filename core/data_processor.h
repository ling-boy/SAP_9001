/**
 * @file data_processor.h
 * @brief 数据处理器接口（责任链模式）
 * @details 定义数据处理流水线，支持链式处理
 *
 * 使用示例：
 *   auto chain = std::make_unique<HJ212Packager>();
 *   chain->setNext(std::make_unique<DataPersister>());
 *   chain->setNext(std::make_unique<DataSender>());
 *   chain->process(sensor_data);
 *
 * @version 1.0
 */
#pragma once

#include <memory>
#include <string>
#include "infra/logger.h"

namespace sap {

/**
 * @brief 数据处理器接口（责任链模式）
 */
class DataProcessor {
public:
    virtual ~DataProcessor() = default;

    /**
     * @brief 处理数据
     * @param data 输入数据
     * @return 处理后的数据（传递给下一个处理器）
     */
    virtual std::string process(std::string data) = 0;

    /**
     * @brief 设置下一个处理器
     */
    void setNext(std::unique_ptr<DataProcessor> next) {
        if (next_) {
            next_->setNext(std::move(next));
        } else {
            next_ = std::move(next);
        }
    }

protected:
    /**
     * @brief 传递给下一个处理器
     */
    std::string passToNext(std::string data) {
        if (next_) {
            return next_->process(std::move(data));
        }
        return data;
    }

private:
    std::unique_ptr<DataProcessor> next_;
};

/**
 * @brief 数据验证处理器
 */
class DataValidator : public DataProcessor {
public:
    std::string process(std::string data) override {
        if (data.empty()) {
            LOG_WARN("processor", "Empty data received, skipping");
            return "";
        }
        // 可以添加更多验证逻辑
        return passToNext(std::move(data));
    }
};

/**
 * @brief 数据格式化处理器
 */
class DataFormatter : public DataProcessor {
public:
    std::string process(std::string data) override {
        // 确保数据以 @ 结尾
        if (!data.empty() && data.back() != '@') {
            data += '@';
        }
        return passToNext(std::move(data));
    }
};

/**
 * @brief 数据持久化处理器
 */
class DataPersister : public DataProcessor {
public:
    std::string process(std::string data) override {
        // 将数据推送到发送队列
        // 实际实现在具体使用时注入
        LOG_DEBUG("processor", "Data persisted: %zu bytes", data.length());
        return passToNext(std::move(data));
    }
};

/**
 * @brief 数据发送处理器
 */
class DataSender : public DataProcessor {
public:
    std::string process(std::string data) override {
        // 实际发送逻辑在具体使用时注入
        LOG_DEBUG("processor", "Data sent: %zu bytes", data.length());
        return passToNext(std::move(data));
    }
};

} // namespace sap
