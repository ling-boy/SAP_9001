# 应用模块 AI Skill

## 模块概述

应用层模块，负责设备初始化、注册管理和消息传输。

## 关键文件

- `app/device_init.h/.cpp` - 设备初始化
- `app/regist_manage.h/.cpp` - 设备注册
- `app/message_transmitter.h/.cpp` - 消息传输

## 设备初始化流程

```
1. 创建通信策略（CommunicationFactory）
2. 初始化各通信设备
3. 注册到 commManager
4. 注册重初始化回调
```

## 注册流程（状态机）

```
SelectChannel → WaitForIdle → SendRegister → WaitFor17
    → SendConfirm → WaitForTimestamp → Success/Failed
```

## 消息传输流程

```
1. 等待网关数据请求（03协议）
2. 通知传感器采集
3. 从队列取出数据包
4. 发送到网关
5. 定时发送心跳
```

## 添加新通信方式

1. 在 `device_init.cpp` 的 `dev_init()` 中添加初始化逻辑
2. 在 `CommunicationFactory` 中注册策略
3. 配置 `config/device.conf`

## 常见问题

### 注册失败
- 检查网络连接
- 检查 MAC 地址是否正确
- 检查网关是否在线

### 数据发送失败
- 检查通信设备状态
- 检查看门狗是否超时
- 检查队列是否满

### 重初始化失败
- 检查熔断器状态
- 检查设备是否可用
- 查看日志定位问题
