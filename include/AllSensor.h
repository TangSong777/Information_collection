#ifndef __AllSensor_H_
#define __AllSensor_H_

#include <Arduino.h>
#include "typedef.h"
#include "SoftwareSerial.h"
#include "DHT.h"
#include <DHT_U.h>
#include <Wire.h>
#include <BH1750.h>

void AllSensor_init();
void DHT11_Getvalue();
void BH1750_Getvalue();
void CZ0015_GetValue();

#endif /* __AllSensor_H_ */