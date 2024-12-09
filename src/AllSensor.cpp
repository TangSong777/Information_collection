#include "AllSensor.h"

SoftwareSerial airSensorSerial(16, 17); // RX, TX
DHT_Unified dht(4, DHT11);
BH1750 lightMeter;

int CZ0015_rec_flag = 0;
uint8_t CZ0015_buffer[25];
int CZ0015_buffer_i = 0;

void AllSensor_init()
{
    Wire.begin();       // BH1750启动
    lightMeter.begin(); // BH1750启动
    // airSensorSerial.begin(9600); // CZ0015启动
    airSensorSerial.begin(9600);
    dht.begin(); // DHT11启动
}
void DHT11_Getvalue()
{
    sensors_event_t event;

    dht.temperature().getEvent(&event);
    if (!isnan(event.temperature))
    {
        sensorData.Temperature = event.temperature;
    }

    dht.humidity().getEvent(&event);
    if (!isnan(event.relative_humidity))
    {
        sensorData.Humidity = event.relative_humidity;
    }
}
void BH1750_Getvalue()
{
    sensorData.Lux = lightMeter.readLightLevel(); // 读取光强度值
}
void CZ0015_GetValue()
{
    if (airSensorSerial.available())
    {
        uint8_t data = airSensorSerial.read();
        CZ0015_buffer[CZ0015_buffer_i++] = data;

        // 检查是否收到了完整的数据包
        if (CZ0015_buffer_i >= 9 && CZ0015_buffer[CZ0015_buffer_i - 9] == 0x2C && CZ0015_buffer[CZ0015_buffer_i - 8] == 0xE4)
        {
            CZ0015_rec_flag = 1;
        }

        // 如果收到完整数据包，处理数据
        if (CZ0015_rec_flag == 1)
        {
            // 这里需要根据传感器数据格式来解析数据
            // 假设数据格式已知，以下为示例解析过程
            sensorData.CH2O = ((float)CZ0015_buffer[CZ0015_buffer_i - 5] * 256 + (float)CZ0015_buffer[CZ0015_buffer_i - 4]) * 0.001;
            sensorData.TVOC = ((float)CZ0015_buffer[CZ0015_buffer_i - 7] * 256 + (float)CZ0015_buffer[CZ0015_buffer_i - 6]) * 0.001;
            sensorData.CO2 = (float)CZ0015_buffer[CZ0015_buffer_i - 3] * 256 + (float)CZ0015_buffer[CZ0015_buffer_i - 2];
            // 清空缓冲区和标志位
            memset(CZ0015_buffer, 0, sizeof(CZ0015_buffer));
            CZ0015_buffer_i = 0;
            CZ0015_rec_flag = 0;
        }
    }
}
