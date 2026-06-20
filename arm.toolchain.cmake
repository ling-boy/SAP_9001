# ARM 交叉编译工具链配置
# 升级到 GCC 13 以支持现代 C++ 特性和 ##__VA_ARGS__ 宏扩展

# 1. 设置目标系统信息
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# 2. 指定交叉编译器 (使用系统安装的 GCC 13)
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc-13)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++-13)

# 3. 设置目标环境的根文件系统路径
set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)

# 4. 设置 find_* 命令的搜索模式
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
