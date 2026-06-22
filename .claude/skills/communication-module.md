# 通信模块 AI Skill

## 模块概述

通信方式管理模块，支持 LoRa、WiFi、蓝牙、LAN、4G 五种通信方式。

## 关键文件

- `core/communication_strategy.h` - 策略接口
- `core/communication_factory.h/.cpp` - 工厂
- `hal/lora_strategy.h` - LoRa 策略
- `hal/wifi_strategy.h` - WiFi 策略
- `hal/bt_strategy.h` - 蓝牙策略
- `hal/lan_strategy.h` - LAN 策略
- `hal/cell4g_strategy.h` - 4G 策略

## 通信方式

| 方式 | 设备ID | 端口 | 说明 |
|------|--------|------|------|
| LoRa | 1 | /dev/tong_lora | 无线通信 |
| WiFi | 2 | wlan0 | 无线网络 |
| BT | 3 | /dev/tong_bt | 蓝牙 |
| LAN | 4 | eth1 | 有线网络 |
| LAN_Server | 5 | eth0 | 服务器模式 |
| 4G | 6 | ppp0 | 蜂窝网络 |

## 添加新通信方式

1. 创建策略类继承 `CommunicationStrategyBase`
2. 实现 `initialize()` 和 `reinitialize()` 方法
3. 在 `CommunicationFactory::createAll()` 中注册

```cpp
class NewStrategy : public CommunicationStrategyBase {
public:
    int initialize() override {
        // 初始化逻辑
        return fd_;
    }
    int reinitialize(int old_fd) override {
        close(old_fd);
        return initialize();
    }
    std::string typeName() const override { return "New"; }
    int deviceId() const override { return 7; }
};
```

## 容错机制

- **熔断器**：连续失败 5 次后熔断 30 秒
- **指数退避**：重试间隔 1s → 2s → 4s → ... → 60s
