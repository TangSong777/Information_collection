#ifndef __UI_H_
#define __UI_H_

#include <Arduino.h>
#include "font.h"
#include "typedef.h"

void oled_init();
void oled_show_Ace();
void oled_show_cnchar(uint8_t x, uint8_t y, uint8_t num);
void m_Home();
void m_Clock();
void m_Alarm();
void m_Environment();
void m_Weather();
void m_About();
void m_Clockopt();
void m_Alarmopt();
void m_Environment_information1();
void m_Environment_information2();
void m_Weather_today1();
void m_Weather_today2();
void m_Weather_tomorrow1();
void m_Weather_tomorrow2();
void m_Weather_day_after_tomorrow1();
void m_Weather_day_after_tomorrow2();
void m_Information();
#endif /* __UI_H_ */