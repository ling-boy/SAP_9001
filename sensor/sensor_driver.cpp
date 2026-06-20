/**
 * @file sensor_driver.cpp
 * @brief 传感器驱动基类实现及船载/大气传感器驱动
 * @details 提供统一的 Modbus RTU 轮询框架，消除 get_ship_data/get_gassensor_data 的重复代码
 */
#include "sensor/sensor_driver.h"
#include "protocol/hj212.h"
#include "protocol/protocol_process.h"
#include "hal/gps.h"
#include "infra/message_queue.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
using namespace std;

/* 外部全局变量（在 modbus_9001.cpp 中定义） */
extern MessageQueue<string> transMessage;
extern pthread_mutex_t mtx_sensor;
extern pthread_cond_t cond_sensor;
extern int iS_getsensor;
extern int iS_getshipsensor;
extern int wdtNewSensorId;
extern int wdtShipId;

/* ====== SensorDriver 基类实现 ====== */

SensorDriver::SensorDriver(CSoftwareWdt* wdt, int wdt_id,
                           const string& port_info, const string& device, int baud)
    : wdt_(wdt), wdt_id_(wdt_id), port_info_(port_info), device_(device), baud_(baud)
{
}

SensorDriver::~SensorDriver()
{
}

/**
 * @brief 将4字节数据转换为32位浮点数（IEEE 754）
 */
static float float2decimal(long int byte0, long int byte1, long int byte2, long int byte3)
{
    long int realbyte0, realbyte1, realbyte2, realbyte3;
    char S;
    long int E;
    double M;
    float D;
    realbyte0 = byte0;
    realbyte1 = byte1;
    realbyte2 = byte2;
    realbyte3 = byte3;
    if ((realbyte0 & 0x80) == 0) {
        S = 0;
    }
    else {
        S = 1;
    }
    E = (((realbyte0 & 0x7F) << 1) | ((realbyte1 & 0x80) >> 7)) - 127;
    M = ((realbyte1 & 0x7f) << 16) | (realbyte2 << 8) | realbyte3;
    D = pow(-1.0, S) * (1.0 + M / pow(2.0, 23)) * pow(2.0, E);
    return D;
}

/** @brief 获取16位数据的高8位 */
static uint8_t int16_t_getH(uint16_t a)
{
    return (a >> 8) & 0xFF;
}

/** @brief 获取16位数据的低8位 */
static uint8_t int16_t_getL(uint16_t a)
{
    return 0xff & a;
}

bool SensorDriver::readSensorValue(modbus_t* ctx, const SensorConfig& config, double& value)
{
    uint16_t tab_reg[4] = { 0 };
    int regs = 0;

    switch (config.conversion) {
    case ConversionType::FLOAT32:
    case ConversionType::FLOAT32_SWAP:
        regs = modbus_read_registers(ctx, config.register_address, config.register_count, tab_reg);
        if (regs == -1) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
            return false;
        }
        {
            uint8_t a, b, c, d;
            if (config.conversion == ConversionType::FLOAT32) {
                c = int16_t_getH(tab_reg[0]);
                d = int16_t_getL(tab_reg[0]);
                a = int16_t_getH(tab_reg[1]);
                b = int16_t_getL(tab_reg[1]);
            }
            else { /* FLOAT32_SWAP */
                d = int16_t_getH(tab_reg[0]);
                c = int16_t_getL(tab_reg[0]);
                b = int16_t_getH(tab_reg[1]);
                a = int16_t_getL(tab_reg[1]);
            }
            value = float2decimal(a, b, c, d);
        }
        return true;

    case ConversionType::UINT16_SCALE:
        regs = modbus_read_registers(ctx, config.register_address, config.register_count, tab_reg);
        if (regs == -1) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
            return false;
        }
        value = tab_reg[0] * config.scale_factor;
        return true;

    case ConversionType::UINT16_RAW:
        regs = modbus_read_registers(ctx, config.register_address, config.register_count, tab_reg);
        if (regs == -1) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
            return false;
        }
        value = tab_reg[0];
        return true;

    case ConversionType::INPUT_REG_FLOAT32:
        regs = modbus_read_input_registers(ctx, config.register_address, config.register_count, tab_reg);
        if (regs == -1) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
            return false;
        }
        {
            uint8_t a = int16_t_getH(tab_reg[0]);
            uint8_t b = int16_t_getL(tab_reg[0]);
            uint8_t c = int16_t_getH(tab_reg[1]);
            uint8_t d = int16_t_getL(tab_reg[1]);
            value = float2decimal(a, b, c, d);
        }
        return true;
    }
    return false;
}

void SensorDriver::run()
{
    vector<SensorConfig> configs = getSensorConfigs();
    string st = getHJ212ST();
    string hj_MN = get_MN();
    string hj_Flag = "4";
    size_t sensor_count = configs.size();
    if (sensor_count == 0) {
        fprintf(stderr, "传感器配置为空，无法启动采集\n");
        return;
    }

    /* 初始化 Modbus RTU */
    modbus_t* ctx = modbus_new_rtu(device_.c_str(), baud_, 'N', 8, 1);
    if (ctx == NULL) {
        fprintf(stderr, "modbus_new_rtu failed\n");
        return;
    }
    modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS485);
    modbus_rtu_set_rts(ctx, MODBUS_RTU_RTS_DOWN);
    modbus_set_debug(ctx, TRUE);
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return;
    }
    cout << "传感器连接成功!" << endl;

    vector<double> sensor_values(sensor_count, -999);
    size_t index_flag = 0;

    while (1) {
        cout << "开始获取传感器" << index_flag + 1 << "数据" << endl;
        string accessdev_gps = gps_get_location();
        modbus_set_slave(ctx, configs[index_flag].slave_address);

        double value = -999;
        if (readSensorValue(ctx, configs[index_flag], value)) {
            sensor_values[index_flag] = value;
            sleep(configs[index_flag].sleep_seconds);
        }
        else {
            cout << "读取传感器" << index_flag + 1 << "数据失败" << endl;
            sleep(4);
        }
        wdt_->KeepSoftwareWdtAlive(wdt_id_);
        printf("thread of sensor feed dog success\n");

        /* 最后一个传感器读取完成后，打包 HJ212 数据 */
        if (index_flag == sensor_count - 1) {
            string hj_QN = time_now_to_string();

            /* 打印所有传感器数据 */
            cout << "--------------------------------" << endl;
            cout << "获取一轮后的数据如下：" << endl;
            for (size_t i = 0; i < sensor_count; i++) {
                printf("%s: %f\n", configs[i].name.c_str(), sensor_values[i]);
            }
            cout << "--------------------------------" << endl;

            /* 构建 HJ212 数据段 */
            string data_segment = "QN=" + hj_QN + ";ST=" + st + ";CN=2011;PW=123456;MN=" + hj_MN +
                                  ";Flag=" + hj_Flag + ";CP=&&Gps=" + accessdev_gps + ";";
            for (size_t i = 0; i < sensor_count; i++) {
                char buf[256];
                string fmt = "%s-Rtd=" + string(configs[i].rtd_format) + ";";
                /* 验证格式字符串中 '%' 出现次数：预期2个（一个 %s，一个浮点格式） */
                int pct_count = 0;
                for (size_t j = 0; j < fmt.size(); j++) {
                    if (fmt[j] == '%') pct_count++;
                }
                if (pct_count != 2) {
                    fprintf(stderr, "Invalid format string for sensor %s: %s\n",
                            configs[i].name.c_str(), fmt.c_str());
                    fmt = "%s-Rtd=%010.3lf;";
                }
                snprintf(buf, sizeof(buf), fmt.c_str(), configs[i].hj212_code.c_str(), sensor_values[i]);
                data_segment += buf;
            }
            data_segment += "&&";

            /* 封装 HJ212 数据包 */
            int len_origin = data_segment.length();
            string data_len = ensureLen_4(len_origin);
            unsigned int hjcrc = CRC16_Checkout((unsigned char*)data_segment.c_str(), len_origin);
            string hj212pacet = ensure_crc4_packet(hjcrc, data_len, data_segment);
            cout << "HJ212数据包:" << hj212pacet << endl;

            /* 封装 06 协议并推送到队列 */
            string data06 = packet06(hj212pacet, port_info_);
            cout << "06数据包:" << data06 << endl;
            transMessage.push(data06);

            /* 等待下一次采集触发 */
            int S = pthread_mutex_lock(&mtx_sensor);
            if (S != 0) {
                perror("pthread_mutex_lock()");
                sleep(4);
                index_flag = (index_flag + 1) % sensor_count;
                continue;
            }
            /* 根据端口信息等待对应的条件变量 */
            if (port_info_ == "100") { /* RS485_A = 船载传感器 */
                while (iS_getshipsensor == 0) {
                    pthread_cond_wait(&cond_sensor, &mtx_sensor);
                }
                iS_getshipsensor = 0;
            }
            else { /* RS485_B = 大气传感器 */
                while (iS_getsensor == 0) {
                    pthread_cond_wait(&cond_sensor, &mtx_sensor);
                }
                iS_getsensor = 0;
            }
            pthread_mutex_unlock(&mtx_sensor);

            /* 重置所有传感器值 */
            for (size_t i = 0; i < sensor_count; i++) {
                sensor_values[i] = -999;
            }
        }

        index_flag = (index_flag + 1) % sensor_count;
    }
}


/* ====== 船载传感器驱动 ====== */

/** @brief 船载传感器配置（13个 Modbus 从站，部分从站多寄存器拆分为独立条目） */
static vector<SensorConfig> getShipSensorConfigs()
{
    return {
        { 1,  1,    2, ConversionType::FLOAT32,       1.0,    "w01010", "溶解氧",    1, "%010.3lf" },
        { 2,  1,    2, ConversionType::FLOAT32,       1.0,    "w21003", "氨氮",      1, "%010.3lf" },
        { 3,  1,    2, ConversionType::FLOAT32,       1000.0, "w20138", "铜离子",    1, "%010.3lf" },
        { 4,  1,    2, ConversionType::FLOAT32,       1.0,    "w20143", "镉离子",    1, "%010.3lf" },
        { 5,  9728, 4, ConversionType::FLOAT32_SWAP,  0.0001, "w00003", "蓝绿藻",    1, "%010.3lf" },
        { 6,  9728, 4, ConversionType::FLOAT32_SWAP,  1.0,    "w00004", "叶绿素",    1, "%010.3lf" },
        { 7,  0,    2, ConversionType::FLOAT32,       1000.0, "w01014", "电导率",    1, "%010.3lf" },
        { 7,  6,    2, ConversionType::FLOAT32,       1.0,    "w01006", "TDS",       1, "%010.3lf" },
        { 8,  0,    1, ConversionType::UINT16_SCALE,  0.1,    "a01002", "环境湿度",  2, "%010.3lf" },
        { 8,  1,    1, ConversionType::UINT16_SCALE,  0.1,    "q00005", "环境温度",  2, "%010.3lf" },
        { 8,  2,    1, ConversionType::UINT16_RAW,    1.0,    "a05001", "CO2",       2, "%010.3lf" },
        { 8,  3,    2, ConversionType::FLOAT32,       1.0,    "a01004", "光照",      2, "%010.3lf" },
        { 9,  0,    1, ConversionType::UINT16_SCALE,  1.0,    "a21001", "NH3",       2, "%010.3lf" },
        { 10, 0,    2, ConversionType::INPUT_REG_FLOAT32, 1.0, "w00007", "深度",     1, "%010.3lf" },
        { 11, 0,    1, ConversionType::UINT16_SCALE,  0.1,    "w01012", "浊度",      1, "%010.3lf" },
        { 12, 0,    1, ConversionType::UINT16_SCALE,  1.0,    "a01008", "风向",      2, "%010.3lf" },
        { 12, 1,    1, ConversionType::UINT16_SCALE,  0.1,    "a01007", "风速",      2, "%010.3lf" },
        { 12, 2,    1, ConversionType::UINT16_RAW,    1.0,    "a01003", "雨量",      2, "%010.3lf" },
        { 12, 3,    1, ConversionType::UINT16_SCALE,  0.1,    "a01004", "光照度",    2, "%010.3lf" },
        { 12, 4,    1, ConversionType::UINT16_SCALE,  0.1,    "a01001", "气象温度",  2, "%010.3lf" },
        { 12, 5,    1, ConversionType::UINT16_SCALE,  0.1,    "a01002", "气象湿度",  2, "%010.3lf" },
        { 12, 6,    1, ConversionType::UINT16_SCALE,  0.1,    "a01006", "大气压力",  2, "%010.3lf" },
        { 13, 0,    1, ConversionType::UINT16_SCALE,  0.1,    "LA",     "噪声",      2, "%010.3lf" },
    };
}

/**
 * @brief 船载传感器驱动类
 */
class ShipSensorDriver : public SensorDriver {
public:
    ShipSensorDriver(CSoftwareWdt* wdt, int wdt_id)
        : SensorDriver(wdt, wdt_id, "100", "/dev/ttymxc2", 9600) {}

    vector<SensorConfig> getSensorConfigs() override {
        return getShipSensorConfigs();
    }

    string getHJ212ST() override {
        return "26";
    }
};

void* get_ship_data(void* arg)
{
    CSoftwareWdt* wdt = (CSoftwareWdt*)arg;
    const char* threadname = "get_ship_data";
    wdtShipId = wdt->RequestSoftwareWdtID(threadname, 30);

    ShipSensorDriver driver(wdt, wdtShipId);
    driver.run();
    return nullptr;
}


/* ====== 大气传感器驱动 ====== */

/** @brief 大气传感器配置（11个 Modbus 从站，部分从站多寄存器拆分为独立条目） */
static vector<SensorConfig> getGasSensorConfigs()
{
    return {
        { 1,  6, 1, ConversionType::UINT16_SCALE, 0.1,  "a21005", "CO",     2, "%08.2lf" },
        { 2,  6, 1, ConversionType::UINT16_SCALE, 0.1,  "a19001", "O2",     2, "%08.2lf" },
        { 4,  6, 1, ConversionType::UINT16_SCALE, 0.1,  "a05002", "CH4",    2, "%08.2lf" },
        { 5,  6, 1, ConversionType::UINT16_SCALE, 0.01, "a34003", "O3",     2, "%08.2lf" },
        { 6,  6, 1, ConversionType::UINT16_SCALE, 0.1,  "a21028", "H2S",    2, "%08.2lf" },
        { 7,  6, 1, ConversionType::UINT16_SCALE, 0.1,  "a21026", "SO2",    2, "%08.2lf" },
        { 8,  6, 1, ConversionType::UINT16_SCALE, 0.1,  "a21001", "NH3",    2, "%08.2lf" },
        { 9,  6, 1, ConversionType::UINT16_SCALE, 0.1,  "a21004", "NO2",    2, "%08.2lf" },
        { 10, 6, 1, ConversionType::UINT16_SCALE, 0.1,  "a21003", "NO",     2, "%08.2lf" },
        { 11, 2, 1, ConversionType::UINT16_RAW,   1.0,  "a34004", "PM2.5",  2, "%08.2lf" },
        { 11, 3, 1, ConversionType::UINT16_RAW,   1.0,  "a34002", "PM10",   2, "%08.2lf" },
        { 11, 4, 1, ConversionType::UINT16_RAW,   1.0,  "a34005", "PM1",    2, "%08.2lf" },
        { 12, 0, 1, ConversionType::UINT16_SCALE, 0.1,  "a01002", "湿度",   2, "%08.2lf" },
        { 12, 1, 1, ConversionType::UINT16_SCALE, 0.1,  "a01001", "温度",   2, "%08.2lf" },
        { 12, 2, 1, ConversionType::UINT16_RAW,   1.0,  "a05001", "CO2",    2, "%08.2lf" },
    };
}

/**
 * @brief 大气传感器驱动类
 */
class GasSensorDriver : public SensorDriver {
public:
    GasSensorDriver(CSoftwareWdt* wdt, int wdt_id)
        : SensorDriver(wdt, wdt_id, "010", "/dev/ttymxc2", 9600) {}

    vector<SensorConfig> getSensorConfigs() override {
        return getGasSensorConfigs();
    }

    string getHJ212ST() override {
        return "31";
    }
};

void* get_gassensor_data(void* arg)
{
    CSoftwareWdt* wdt = (CSoftwareWdt*)arg;
    const char* threadname = "get_gassensor_data";
    wdtNewSensorId = wdt->RequestSoftwareWdtID(threadname, 40);

    GasSensorDriver driver(wdt, wdtNewSensorId);
    driver.run();
    return nullptr;
}
