# SAP_9001 系统级 AI Skill

## 项目概述

SAP_9001 V3.0 企业级物联网网关系统，基于 ARM Linux 平台，用于环境监测领域。

## 技术栈

- 语言：C++14
- 编译器：GCC 13 (ARM 交叉编译)
- 构建系统：CMake 3.16+ / Ninja
- 平台：ARM Linux (i.MX6UL)
- 协议：Modbus RTU + HJ212

## 编译命令

```bash
# Debug 版本
cmake --preset linux-arm-debug
cmake --build --preset linux-arm-debug

# Release 版本
cmake --preset linux-arm-release
cmake --build --preset linux-arm-release
```

## 代码规范

1. **日志规范**：使用 LOG_* 宏，必须指定模块名
   ```cpp
   LOG_INFO("sensor", "Temperature: %.2f C", temp);
   LOG_ERROR("comm", "Connection failed: %s", strerror(errno));
   ```

2. **配置规范**：使用 CFG_STR/CFG_INT 读取配置
   ```cpp
   std::string ip = CFG_STR("network.wifi", "server_ip", "192.168.3.1");
   int port = CFG_INT("network.wifi", "server_port", 1234);
   ```

3. **内存管理**：使用智能指针，避免 raw new/delete
   ```cpp
   auto node = std::make_unique<communicateNode>();
   ```

4. **线程安全**：共享数据使用 mutex 保护
   ```cpp
   std::lock_guard<std::mutex> lock(mtx_);
   ```

## 设计模式

| 模式 | 应用位置 |
|------|---------|
| 单例 | DeviceContext, Logger, Config |
| 工厂 | CommunicationFactory |
| 策略 | ICommunicationStrategy |
| 建造者 | HJ212PacketBuilder |
| 状态机 | device_regist() |
| 模板方法 | SensorDriver |

## 目录结构

```
core/       - 核心框架（单例、工厂、策略）
infra/      - 基础设施（日志、配置、内存池、熔断器）
protocol/   - 协议层（HJ212、私有协议）
sensor/     - 传感器层（驱动、采集）
hal/        - 硬件抽象层（LoRa/WiFi/BT/LAN/4G）
app/        - 应用层（初始化、注册、传输）
```

## Bug 检查清单

1. ✅ 编译是否通过
2. ✅ 是否有内存泄漏
3. ✅ 是否有线程安全问题
4. ✅ 是否有资源泄漏（fd、socket）
5. ✅ 日志是否规范
6. ✅ 配置是否外部化
