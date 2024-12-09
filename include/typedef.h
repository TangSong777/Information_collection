#ifndef __TYPEDEF_H_
#define __TYPEDEF_H_

#include <Arduino.h>

typedef struct // 时钟
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t opt;
} CLOCK_TypeDef;
extern CLOCK_TypeDef clocktime;

typedef struct // 闹钟
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t opt;
} ALARM_TypeDef;
extern ALARM_TypeDef alarmtime;

typedef struct
{
    uint8_t current;                 // 当前状态索引号
    uint8_t enter;                   // 确定
    uint8_t next;                    // 向下一个
    uint8_t back;                    // 退出
    void (*current_operation)(void); // 当前状态应该执行的操作
} MenuTable_Typedef;
extern MenuTable_Typedef table[30];

typedef struct
{
    float Temperature = 0.0;
    float Humidity = 0.0;
    int Lux = 0;
    float TVOC, CH2O, CO2;
} AllSensorData_Typedef;
extern AllSensorData_Typedef sensorData;

struct WeatherInfo
{
    String date;
    int week;
    String dayWeather;
    int dayTemp;
};

// 阵雨 多云 晴 阴 小雨

#endif /* __TYPEDEF_H_ */