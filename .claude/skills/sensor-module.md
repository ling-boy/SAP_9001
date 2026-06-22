# 传感器模块 AI Skill

## 模块概述

传感器数据采集模块，负责通过 Modbus RTU 协议读取传感器数据。

## 关键文件

- `sensor/sensor_driver.h/.cpp` - 传感器驱动基类
- `sensor/usart.h/.cpp` - RS485 监听

## 传感器配置

### 船载传感器（23个）
- 端口：RS485_A (port_info="100")
- 设备：/dev/ttymxc2
- ST=26

### 大气传感器（15个）
- 端口：RS485_B (port_info="010")
- 设备：/dev/ttymxc2
- ST=31

## 数据流程

```
Modbus RTU → readSensorValue() → HJ212封装 → MessageQueue
```

## 常见任务

### 添加新传感器

1. 在 `sensor/sensor_driver.cpp` 中添加配置：
```cpp
{ slave_addr, reg_addr, reg_count, ConversionType::FLOAT32, scale, "hj212_code", "name", sleep_sec, "%format" }
```

2. 重新编译部署

### 修改采集频率

修改 `SensorConfig` 中的 `sleep_seconds` 字段。

## Bug 检查

1. ✅ Modbus 从站地址是否正确
2. ✅ 寄存器地址和数量是否匹配
3. ✅ 数据转换方式是否正确
4. ✅ HJ212 编码是否唯一
5. ✅ 采集周期是否合理
