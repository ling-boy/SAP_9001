# 协议模块 AI Skill

## 模块概述

协议处理模块，实现私有通信协议和 HJ212 协议。

## 关键文件

- `protocol/protocol_process.h/.cpp` - 协议处理
- `protocol/hj212_builder.h` - HJ212 建造者
- `protocol/hj212.h/.cpp` - HJ212 工具
- `protocol/constants.h` - 协议常量

## 私有协议格式

```
$ + 协议号(2位) + 通信类型(1位) + 设备ID(2位) + 网络ID(4位) + 00 + 长度(4位) + 数据 + @
```

## 协议号

| 协议号 | 说明 | 方向 |
|--------|------|------|
| 01 | 注册请求 | 设备→网关 |
| 02 | 注册确认 | 设备→网关 |
| 03 | 数据请求 | 网关→设备 |
| 06 | 传感器数据 | 设备→网关 |
| 17 | 网关参数 | 网关→设备 |
| 20 | 时间戳 | 网关→设备 |
| EF | 设备心跳 | 设备→网关 |

## HJ212 协议

```
## + 长度(4位) + 数据段 + CRC16(4位)
```

数据段格式：
```
QN=时间戳;ST=类型;CN=2011;PW=密码;MN=设备号;Flag=标志;CP=&&数据&&
```

## 构建数据包

```cpp
// 使用建造者模式
auto packet = HJ212PacketBuilder()
    .fromContext()
    .withData(sensor_data)
    .withPortInfo("100")
    .build();
```

## 添加新协议

1. 在 `constants.h` 中定义协议号
2. 在 `protocol_process.cpp` 中添加处理函数
3. 在 `message_transmitter.cpp` 中添加分发逻辑
