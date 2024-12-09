// 共1123行代码
#include "UI.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "AllSensor.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "weather.h"

Adafruit_SSD1306 oled(128, 64, &Wire);

extern struct WeatherInfo weatherInfos[3];
CLOCK_TypeDef clocktime = {0, 0, 0, 0};
ALARM_TypeDef alarmtime = {11, 45, 14, 0};
AllSensorData_Typedef sensorData;
MenuTable_Typedef table[30] =
    {
        {0, 1, 0, 0, (*m_Home)}, // 一级界面（主页面）：索引，确定，向下一个，退出

        {1, 6, 2, 0, (*m_Clock)},       // 二级界面：时钟信息
        {2, 7, 3, 0, (*m_Alarm)},       // 二级界面：闹钟信息
        {3, 8, 4, 0, (*m_Environment)}, // 二级界面：环境
        {4, 10, 5, 0, (*m_Weather)},    // 二级界面：天气
        {5, 16, 1, 0, (*m_About)},      // 二级界面：作者信息

        {6, 6, 6, 1, (*m_Clockopt)},                       // 三级界面：时钟调整
        {7, 7, 7, 2, (*m_Alarmopt)},                       // 三级界面：闹钟调整
        {8, 8, 9, 3, (*m_Environment_information1)},       // 三级界面：温湿度信息
        {9, 9, 8, 3, (*m_Environment_information2)},       // 三级界面：气体、光照信息
        {10, 10, 12, 4, (*m_Weather_today1)},              // 三级界面：今天天气1
        {11, 11, 12, 4, (*m_Weather_today2)},              // 三级界面：今天天气2
        {12, 12, 14, 4, (*m_Weather_tomorrow1)},           // 三级界面：明天天气1
        {13, 13, 14, 4, (*m_Weather_tomorrow2)},           // 三级界面：明天天气2
        {14, 14, 10, 4, (*m_Weather_day_after_tomorrow1)}, // 三级界面：后天天气1
        {15, 15, 10, 4, (*m_Weather_day_after_tomorrow2)}, // 三级界面：后天天气2
        {16, 16, 16, 5, (*m_Information)},                 // 三级界面：作者信息展示

        // 四级界面 天气的三级界面为今天 明天 后天（包括日期和星期）确认进入四级界面
};

uint8_t wifiWaitPoint = 0;
bool ifGetWeather = false;

void oled_init()
{
    Wire.begin(21, 22, 10000);
    oled.begin(SSD1306_SWITCHCAPVCC, 0x3c);
    oled.clearDisplay();
    oled.display();
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(2);
}
void oled_show_cnchar(uint8_t x, uint8_t y, uint8_t num)
{
    oled.drawBitmap(x, y, CNchar[num], 16, 16, 1);
}
void m_Home()
{
    oled.clearDisplay();
    oled.drawBitmap(32, 0, BMP_Ace, 64, 64, 1);
    if (WiFi.status() == WL_CONNECTED)
    {
        oled.setCursor(0, 0);
        oled.setTextSize(1);
        oled.printf("yes");
        oled.setTextSize(2);
    }
    else
    {
        delay(1000);
        switch (++wifiWaitPoint)
        {
        case 1:
            oled.setCursor(0, 0);
            oled.setTextSize(1);
            oled.printf(".");
            oled.setTextSize(2);
            break;
        case 2:
            oled.setCursor(0, 0);
            oled.setTextSize(1);
            oled.printf("..");
            oled.setTextSize(2);
            break;
        case 3:
            oled.setCursor(0, 0);
            oled.setTextSize(1);
            oled.printf("...");
            oled.setTextSize(2);
            break;
        default:
            wifiWaitPoint = 0;
            break;
        }
    }
    oled.display();
}
void m_Clock()
{
    // oled.clearDisplay();
    // oled.drawBitmap(32, 0, BMP_Clock, 64, 64, 1);
    // oled.display();
    oled.clearDisplay();
    oled.setCursor(16, 24);
    oled.printf("%02d:%02d:%02d", clocktime.hour, clocktime.minute, clocktime.second);
    oled.display();
}
void m_Alarm()
{
    oled.clearDisplay();
    oled.setCursor(16, 24);
    oled.printf("%02d:%02d:%02d", alarmtime.hour, alarmtime.minute, alarmtime.second);
    oled.display();
}
void m_Environment()
{
    oled.clearDisplay();
    oled.drawBitmap(32, 0, BMP_Temp, 64, 64, 1);
    oled.display();
}
void m_Weather()
{
    oled.clearDisplay();
    oled.drawBitmap(32, 0, BMP_Weather, 64, 64, 1);
    oled.display();
}
void m_About()
{
    oled.clearDisplay();
    oled.drawBitmap(32, 0, BMP_Setting, 64, 64, 1);
    oled.display();
}
void m_Clockopt()
{
    oled.clearDisplay();
    oled.setCursor(16, 24);
    switch (clocktime.opt)
    {
    case 0:
        oled.setCursor(16, 24);
        oled.printf("%02d:%02d:%02d", clocktime.hour, clocktime.minute, clocktime.second);
        oled.setTextColor(BLACK, WHITE);
        oled.setCursor(16, 24);
        oled.printf("%02d", clocktime.hour);
        oled.setTextColor(SSD1306_WHITE);
        break;
    case 1:
        oled.setCursor(16, 24);
        oled.printf("%02d:%02d:%02d", clocktime.hour, clocktime.minute, clocktime.second);
        oled.setTextColor(BLACK, WHITE);
        oled.setCursor(52, 24);
        oled.printf("%02d", clocktime.minute);
        oled.setTextColor(SSD1306_WHITE);
        break;
    case 2:
        oled.setCursor(16, 24);
        oled.printf("%02d:%02d:%02d", clocktime.hour, clocktime.minute, clocktime.second);
        oled.setTextColor(BLACK, WHITE);
        oled.setCursor(88, 24);
        oled.printf("%02d", clocktime.second);
        oled.setTextColor(SSD1306_WHITE);
        break;
    default:
        break;
    }
    oled.display();
}
void m_Alarmopt()
{
    oled.clearDisplay();
    oled.setCursor(16, 24);
    switch (alarmtime.opt)
    {
    case 0:
        oled.setCursor(16, 24);
        oled.printf("%02d:%02d:%02d", alarmtime.hour, alarmtime.minute, alarmtime.second);
        oled.setTextColor(BLACK, WHITE);
        oled.setCursor(16, 24);
        oled.printf("%02d", alarmtime.hour);
        oled.setTextColor(SSD1306_WHITE);
        break;
    case 1:
        oled.setCursor(16, 24);
        oled.printf("%02d:%02d:%02d", alarmtime.hour, alarmtime.minute, alarmtime.second);
        oled.setTextColor(BLACK, WHITE);
        oled.setCursor(52, 24);
        oled.printf("%02d", alarmtime.minute);
        oled.setTextColor(SSD1306_WHITE);
        break;
    case 2:
        oled.setCursor(16, 24);
        oled.printf("%02d:%02d:%02d", alarmtime.hour, alarmtime.minute, alarmtime.second);
        oled.setTextColor(BLACK, WHITE);
        oled.setCursor(88, 24);
        oled.printf("%02d", alarmtime.second);
        oled.setTextColor(SSD1306_WHITE);
        break;
    default:
        break;
    }
    oled.display();
}
void m_Environment_information1()
{
    DHT11_Getvalue();
    BH1750_Getvalue();
    oled.clearDisplay();
    oled_show_cnchar(0, 0, 8);
    oled_show_cnchar(16, 0, 13);
    oled.setCursor(32, 0);
    oled.printf(":%2.2f", sensorData.Temperature);
    oled_show_cnchar(0, 16, 9);
    oled_show_cnchar(16, 16, 13);
    oled.setCursor(32, 16);
    oled.printf(":%2.2f%%", sensorData.Humidity);
    oled_show_cnchar(0, 32, 10);
    oled_show_cnchar(16, 32, 12);
    oled.setCursor(32, 32);
    oled.printf(":%d", sensorData.Lux);
    oled.display();
}
void m_Environment_information2()
{
    CZ0015_GetValue();
    oled.clearDisplay();
    // oled_show_cnchar(0, 0, 14);
    // oled_show_cnchar(16, 0, 15);
    oled.setCursor(0, 0);
    oled.printf("TVOC:%1.2f2", sensorData.TVOC * 100);
    oled.setCursor(0, 16);
    oled.printf("CH2O:%1.2f", sensorData.CH2O * 100);
    oled.setCursor(0, 32);
    oled.printf("CO2:%fpp", sensorData.CO2);
    oled.display();
}
void m_Weather_today1()
{
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setCursor(0, 0);
    oled.print(weatherInfos[0].date);
    oled.setCursor(0, 16);
    oled.printf("week:%d", weatherInfos[0].week);
    if (weatherInfos[0].dayWeather.equals("晴"))
    {
        oled.setCursor(0, 32);
        oled.printf("sunny");
    }
    else if (weatherInfos[0].dayWeather.equals("阵雨") || weatherInfos[0].dayWeather.equals("小雨"))
    {
        oled.setCursor(0, 32);
        oled.printf("rainy");
    }
    else if (weatherInfos[0].dayWeather.equals("阴") || weatherInfos[0].dayWeather.equals("多云"))
    {
        oled.setCursor(0, 32);
        oled.printf("cloudy");
    }
    oled.setCursor(0, 48);
    oled.printf("daytemp:%d", weatherInfos[0].dayTemp);
    oled.display();
}
void m_Weather_today2()
{
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.printf("Weather2");
    oled.display();
}
void m_Weather_tomorrow1()
{
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setCursor(0, 0);
    oled.print(weatherInfos[1].date);
    oled.setCursor(0, 16);
    oled.printf("week:%d", weatherInfos[1].week);
    if (weatherInfos[1].dayWeather.equals("晴"))
    {
        oled.setCursor(0, 32);
        oled.printf("sunny");
    }
    else if (weatherInfos[1].dayWeather.equals("阵雨") || weatherInfos[1].dayWeather.equals("小雨"))
    {
        oled.setCursor(0, 32);
        oled.printf("rainy");
    }
    else if (weatherInfos[1].dayWeather.equals("阴") || weatherInfos[1].dayWeather.equals("多云"))
    {
        oled.setCursor(0, 32);
        oled.printf("cloudy");
    }
    oled.setCursor(0, 48);
    oled.printf("daytemp:%d", weatherInfos[1].dayTemp);
    oled.display();
}
void m_Weather_tomorrow2()
{
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.printf("Weather4");
    oled.display();
}
void m_Weather_day_after_tomorrow1()
{
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setCursor(0, 0);
    oled.print(weatherInfos[2].date);
    oled.setCursor(0, 16);
    oled.printf("week:%d", weatherInfos[2].week);
    if (weatherInfos[2].dayWeather.equals("晴"))
    {
        oled.setCursor(0, 32);
        oled.printf("sunny");
    }
    else if (weatherInfos[2].dayWeather.equals("阵雨") || weatherInfos[2].dayWeather.equals("小雨"))
    {
        oled.setCursor(0, 32);
        oled.printf("rainy");
    }
    else if (weatherInfos[2].dayWeather.equals("阴") || weatherInfos[2].dayWeather.equals("多云"))
    {
        oled.setCursor(0, 32);
        oled.printf("cloudy");
    }
    oled.setCursor(0, 48);
    oled.printf("daytemp:%d", weatherInfos[2].dayTemp);
    oled.display();
}
void m_Weather_day_after_tomorrow2()
{
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.printf("Weather6");
    oled.display();
}
void m_Information()
{
    oled.clearDisplay();
    oled_show_cnchar(0, 0, 25);
    oled_show_cnchar(16, 0, 26);
    oled_show_cnchar(32, 0, 27); // 制作者

    oled_show_cnchar(0, 16, 18);
    oled_show_cnchar(16, 16, 19);
    oled_show_cnchar(32, 16, 20); // 徐梓轩

    oled_show_cnchar(0, 32, 21);
    oled_show_cnchar(16, 32, 22);
    oled_show_cnchar(32, 32, 23); // 王廷毅

    oled_show_cnchar(0, 48, 21);
    oled_show_cnchar(16, 48, 24); // 王栋
    oled.display();
}