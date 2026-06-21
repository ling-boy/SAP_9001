# 文件名: CMakeLists.txt

# 1. 设置 CMake 最低版本和项目信息
cmake_minimum_required(VERSION 3.16)
project(SAP_Project LANGUAGES C CXX)

# 2. 设置 C++ 标准（升级到 C++14，GCC 4.9.4 完整支持）
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 3. 自动查找源文件（使用 GLOB 而非 GLOB_RECURSE，避免扫描到 build 目录）
file(GLOB SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/hal/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/infra/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/protocol/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/sensor/*.cpp"
)

# 4. 定义最终生成的可执行文件名，并关联源文件
add_executable(modbus_9001 ${SOURCES})

# 5. 指定头文件搜索路径
target_include_directories(modbus_9001 PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"              # 项目根目录（hal/, infra/, protocol/ 等）
    "${CMAKE_CURRENT_SOURCE_DIR}/libmodbus/include" # libmodbus 的头文件
)

# 6. 指定库文件搜索路径
target_link_directories(modbus_9001 PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/libmodbus/lib" # libmodbus 的库文件
)

# 7. 链接所需的库（使用静态库避免 glibc 版本问题）
target_link_libraries(modbus_9001
    ${CMAKE_CURRENT_SOURCE_DIR}/libmodbus/lib/libmodbus.a  # 静态链接 libmodbus
    pthread   # 链接多线程库
    m         # 链接数学库
)

# 8. 完全静态链接，避免目标设备 glibc 版本不兼容
target_link_options(modbus_9001 PRIVATE
    -static              # 完全静态链接（包括 libc, libm, libstdc++）
)

# 8. (可选) 添加一些编译选项
target_compile_options(modbus_9001 PRIVATE -Wall -Wextra)

# 9. 复制配置文件到构建目录
file(GLOB CONFIG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/config/*")
file(COPY ${CONFIG_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/config)

# 10. 打印输出信息
message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Sources: ${SOURCES}")
