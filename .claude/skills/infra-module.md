# 基础设施模块 AI Skill

## 模块概述

基础设施模块，提供日志、配置、内存池、熔断器等通用组件。

## 关键文件

- `infra/logger.h` - 日志库
- `infra/config.h` - 配置管理器
- `infra/memory_pool.h` - 内存池
- `infra/circuit_breaker.h` - 熔断器
- `infra/retry_policy.h` - 指数退避
- `infra/message_queue.h` - 线程安全队列
- `infra/software_wdt.h/.cpp` - 软件看门狗
- `infra/communica_manage.h/.cpp` - 通信设备管理

## 日志系统

```cpp
LOG_TRACE("module", "message");
LOG_DEBUG("module", "message");
LOG_INFO("module", "message");
LOG_WARN("module", "message");
LOG_ERROR("module", "message");
LOG_FATAL("module", "message");
```

## 配置管理

```cpp
std::string value = CFG_STR("section", "key", "default");
int num = CFG_INT("section", "key", 0);
```

## 内存池

```cpp
// 创建内存池
MemoryPool<PacketBuffer, 8> pool;

// 分配
auto* buf = pool.allocate();

// 释放
pool.deallocate(buf);
```

## 熔断器

```cpp
CircuitBreaker breaker(5, 30000);  // 5次失败，30秒冷却

if (breaker.allowRequest()) {
    // 执行操作
    if (success) breaker.recordSuccess();
    else breaker.recordFailure();
}
```

## 指数退避

```cpp
ExponentialBackoff backoff(1000, 60000);  // 1秒基础，60秒最大

int delay = backoff.nextDelay();  // 1s → 2s → 4s → ...
usleep(delay * 1000);
backoff.reset();  // 成功后重置
```
