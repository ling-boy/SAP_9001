
## 🚀 快速开始

### 环境要求

- **操作系统**：Linux (Ubuntu 20.04+)
- **编译器**：GCC 13 ARM 交叉编译工具链
- **构建工具**：CMake 3.16+, Ninja

### 安装工具链

```bash
# Ubuntu/Debian
sudo apt-get install gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf
sudo apt-get install cmake ninja-build
```

### 编译项目

```bash
# 克隆项目
git clone <repository-url>
cd SAP_9001_V2.1

# Debug 版本
cmake --preset linux-arm-debug
cmake --build --preset linux-arm-debug

# Release 版本
cmake --preset linux-arm-release
cmake --build --preset linux-arm-release
```

### 输出文件

- Debug: `build/modbus_9001`
- Release: `build-release/modbus_9001`

### 部署到设备

```bash
# 复制到设备
scp build-release/modbus_9001 root@<device-ip>:/home/root/
scp config/device.conf root@<device-ip>:/home/root/config/

# 在设备上运行
ssh root@<device-ip>
cd /home/root
./modbus_9001
```

## ⚙️ 配置说明

配置文件位于 `config/device.conf`，使用 INI 格式：

```ini
[device]
id = FF
net_id = 0000

[network.wifi]
server_ip = 192.168.3.1
server_port = 1234

[network.lan]
server_ip = 192.168.2.1
server_port = 2234

[serial.gps]
port = /dev/ttymxc7
baudrate = 9600
init_delay_sec = 60

[watchdog]
pid_file = /home/root/myWatch.txt
timeout_sec = 30
```

## 📊 日志系统

日志使用统一格式，支持 6 个级别：

```
[2026-06-21 14:30:15.123] [INFO ] [sensor  ] [T:1234] Temperature: 25.50°C
[2026-06-21 14:30:15.456] [ERROR] [comm    ] [T:1234] Connection failed
```

| 级别 | 用途 |
|------|------|
| TRACE | 最详细的追踪信息 |
| DEBUG | 调试信息 |
| INFO | 关键业务事件 |
| WARN | 可恢复的异常 |
| ERROR | 不可忽略的错误 |
| FATAL | 致命错误 |





## 📦 依赖

| 依赖 | 版本 | 用途 |
|------|------|------|
| libmodbus | 3.1.6 | Modbus RTU 通信 |
| pthread | - | 多线程支持 |
| GCC | 13+ | C++14 编译器 |
| CMake | 3.16+ | 构建系统 |

## 🤝 贡献指南

1. Fork 项目
2. 创建功能分支 (`git checkout -b feature/xxx`)
3. 提交更改 (`git commit -m 'feat: add xxx'`)
4. 推送到分支 (`git push origin feature/xxx`)
5. 创建 Pull Request

### 提交规范

```
feat: 新功能
fix: 修复 bug
docs: 文档更新
refactor: 代码重构
test: 测试相关
chore: 构建/工具相关
```

