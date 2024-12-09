#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <BluetoothSerial.h>
#include <ArduinoMqttClient.h>
#include "KEY.h"
#include "UI.h"
#include "typedef.h"
#include "AllSensor.h"
#include "weather.h"
#include <TimeLib.h>

#define LED 2 // 板载LED

BluetoothSerial SerialBT;
hw_timer_t *tim0 = NULL;
hw_timer_t *tim1 = NULL;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
/*pv begin*/
bool sendData = true;
uint8_t func_index = 0;

uint8_t Key0_flag = 0;
uint8_t Key1_flag = 0;
uint8_t Key2_flag = 0;

const char *wifi_ssid = "YHL_Wifi";
const char *wifi_password = "xzx123456789";

extern CLOCK_TypeDef clocktime;

const char broker[] = "iot-06z00j5k1eeru52.mqtt.iothub.aliyuncs.com";
int port = 1883;

const char outTopic[] = "/sys/k1k5fyjAwle/ESP32_dev/thing/event/property/post";

const long interval = 10000;
unsigned long previousMillis = 0;

String inputString = "";

void onMqttMessage(int messageSize)
{
  // we received a message, print out the topic and contents
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', duplicate = ");
  Serial.print(mqttClient.messageDup() ? "true" : "false");
  Serial.print(", QoS = ");
  Serial.print(mqttClient.messageQoS());
  Serial.print(", retained = ");
  Serial.print(mqttClient.messageRetain() ? "true" : "false");
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available())
  {
    char inChar = (char)mqttClient.read();
    inputString += inChar;
    if (inputString.length() == messageSize)
    {

      DynamicJsonDocument json_msg(1024);
      DynamicJsonDocument json_item(1024);
      DynamicJsonDocument json_value(1024);

      deserializeJson(json_msg, inputString);

      String items = json_msg["items"];

      deserializeJson(json_item, items);
      String led = json_item["led"];

      deserializeJson(json_value, led);
      bool value = json_value["value"];

      if (value == 0)
      {
        // 关
        Serial.println("off");
        digitalWrite(4, HIGH);
      }
      else
      {
        // 开
        Serial.println("on");
        digitalWrite(4, LOW);
      }
      inputString = "";
    }
  }
  Serial.println();

  Serial.println();
}
/*pv end*/

// put function declarations here:
void (*current_operation_index)();

void wifi_init();
void service_tim0();
void service_tim1();
void timer_init();
void BT_receivedata_process();
void timeAdd();
void timeChoose();
void timeStop();
void timeStart();
void timeOptSet();
void mqttOnline();
void timeGet();
String getTimeFromHttp();
void parseTimeFromJson(String jsonString);

void setup()
{
  pinMode(25, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  pinMode(27, INPUT_PULLUP);
  Key_Init(KEY0, 25, PULL_UP, &Key0_flag);
  Key_Init(KEY1, 26, PULL_UP, &Key1_flag);
  Key_Init(KEY2, 27, PULL_UP, &Key2_flag); // 初始化按键
  oled_init();
  // Serial.begin(115200);
  // 串口初始化(用于测试)
  wifi_init(); // 用于wifi的连接
  while (WiFi.status() != WL_CONNECTED)
    ;
  get_weather_forecast();
  SerialBT.begin("Hanlin04");
  AllSensor_init(); // 传感器初始化

  // 从HTTP API获取时间
  String response = getTimeFromHttp();
  if (!response.isEmpty())
  {
    // 解析JSON响应以获取时间
    parseTimeFromJson(response);

    timer_init(); // 定时器启动

    mqttClient.setId("k1k5fyjAwle.ESP32_dev|securemode=2,signmethod=hmacsha256,timestamp=1726839854461|");
    mqttClient.setUsernamePassword("ESP32_dev&k1k5fyjAwle", "33db23e3bcb965b4f91063005a05c6124c23c942594d758b869b3497c2136ebf");

    if (!mqttClient.connect(broker, port))
    {
      Serial.print("MQTT connection failed! Error code = ");
      Serial.println(mqttClient.connectError());
      while (1)
        ;
    }
    mqttClient.onMessage(onMqttMessage);
  }
}
void loop()
{
  //

  mqttClient.poll();
  switch (Key_read())
  {
  case KEY0:
    if (func_index == 8)
    {
      mqttOnline();
      BT_receivedata_process();
    }
    timeAdd();
    func_index = table[func_index].enter; // 按键enter按下后的索引号
    timeStop();
    break;
  case KEY1:
    timeChoose();
    func_index = table[func_index].next; // 按键next按下后的索引号
    break;
  case KEY2:
    timeStart();
    timeOptSet();
    func_index = table[func_index].back; // 按键back按下后的索引号
    break;
  default:
    break;
  }
  current_operation_index = table[func_index].current_operation; // 执行当前索引号所对应的功能函数
  (*current_operation_index)();
}

// put function definitions here:
void wifi_init() // 用于wifi的连接
{
  WiFi.begin(wifi_ssid, wifi_password);
}
void service_tim0() // 用于时钟走时1s
{
  clocktime.second++;
  if (clocktime.second == 60)
  {
    clocktime.second = 0;
    clocktime.minute++;
    if (clocktime.minute == 60)
    {
      clocktime.minute = 0;
      clocktime.hour++;
      if (clocktime.hour == 24)
        clocktime.hour = 0;
    }
  }
  // Serial.printf("Time:%02d:%02d:%02d.\r\n", clocktime.hour, clocktime.minute, clocktime.second);
}
void service_tim1() // 用于按键检测0.01s
{
  Key_scan(KEY0);
  Key_scan(KEY1);
  Key_scan(KEY2);
}
void timer_init() // 初始化两个定时器
{
  tim0 = timerBegin(0, 80, true);
  timerAttachInterrupt(tim0, service_tim0, true);
  timerAlarmWrite(tim0, 1000000, true);
  timerAlarmEnable(tim0);

  tim1 = timerBegin(1, 80, true);
  timerAttachInterrupt(tim1, service_tim1, true);
  timerAlarmWrite(tim1, 10000, true);
  timerAlarmEnable(tim1);
}
void BT_receivedata_process() // 用于蓝牙接收数据的处理
{
  if (SerialBT.connected())
  {
    SerialBT.println("Temp:" + (String)sensorData.Temperature);
    SerialBT.println("Humi:" + (String)sensorData.Humidity);
    SerialBT.println("Lux:" + (String)sensorData.Lux);
  }
}
void timeAdd()
{
  if (func_index == 6)
  {
    switch (clocktime.opt)
    {
    case 0:
      clocktime.hour = ++clocktime.hour % 24;
      break;
    case 1:
      clocktime.minute = ++clocktime.minute % 60;
      break;
    case 2:
      clocktime.second = ++clocktime.second % 60;
      break;
    default:
      break;
    }
  }
  if (func_index == 7)
  {
    switch (alarmtime.opt)
    {
    case 0:
      alarmtime.hour = ++alarmtime.hour % 24;
      break;
    case 1:
      alarmtime.minute = ++alarmtime.minute % 60;
      break;
    case 2:
      alarmtime.second = ++alarmtime.second % 60;
      break;
    default:
      break;
    }
  }
}
void timeChoose()
{
  if (func_index == 6)
    clocktime.opt = ++clocktime.opt % 3;
  if (func_index == 7)
    alarmtime.opt = ++alarmtime.opt % 3;
}
void timeStop()
{
  if (func_index == 6 && timerAlarmEnabled(tim0))
    timerAlarmDisable(tim0);
}
void timeStart()
{
  if (func_index == 6 && !timerAlarmEnabled(tim0))
    timerAlarmEnable(tim0);
}
void timeOptSet()
{
  if (func_index == 6 || func_index == 7)
  {
    clocktime.opt = alarmtime.opt = 0;
  }
}
void mqttOnline()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    // save the last time a message was sent
    previousMillis = currentMillis;

    String payload;

    //{"params":{"temp":1.22,"humi":22},"version":"1.0.0"}

    DynamicJsonDocument json_msg(512);
    DynamicJsonDocument json_data(512);

    json_data["Temp"] = (float)sensorData.Temperature;
    json_data["Humi"] = (float)sensorData.Humidity;
    json_data["Lux"] = (int)sensorData.Lux;

    json_msg["params"] = json_data;
    json_msg["version"] = "1.0.0";

    serializeJson(json_msg, payload);

    Serial.print("Sending message to topic: ");
    Serial.println(outTopic);
    Serial.println(payload);

    bool retained = false;
    int qos = 1;
    bool dup = false;

    mqttClient.beginMessage(outTopic, payload.length(), retained, qos, dup);
    mqttClient.print(payload);
    mqttClient.endMessage();

    Serial.println();
  }
}
String getTimeFromHttp()
{
  String timeStr;
  HTTPClient http;
  const char *apiURL = "http://worldtimeapi.org/api/ip";

  http.begin(apiURL);
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0)
  { // HTTP响应成功
    timeStr = http.getString();
  }
  else
  {
    Serial.printf("Error code: %s", http.errorToString(httpResponseCode).c_str());
  }

  http.end(); // 关闭连接
  return timeStr;
}
void parseTimeFromJson(String jsonString)
{
  int idxStart = jsonString.indexOf("\"datetime\":\"") + 12;
  int idxEnd = jsonString.indexOf("\"", idxStart);
  String datetimeStr = jsonString.substring(idxStart, idxEnd);

  // 解析日期时间字符串
  struct tm timeinfo;
  sscanf(datetimeStr.c_str(), "%d-%d-%dT%d:%d:%d",
         &timeinfo.tm_year, &timeinfo.tm_mon, &timeinfo.tm_mday,
         &timeinfo.tm_hour, &timeinfo.tm_min, &timeinfo.tm_sec);

  // 调整tm_year和tm_mon
  timeinfo.tm_year -= 1900; // tm_year is years since 1900
  timeinfo.tm_mon--;        // tm_mon is 0-based

  // 更新当前时间结构体
  clocktime.hour = timeinfo.tm_hour;
  clocktime.minute = timeinfo.tm_min;
  clocktime.second = timeinfo.tm_sec;

  // 输出当前时间
}
