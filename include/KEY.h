#ifndef __KEY_H_
#define __KEY_H_

#include <Arduino.h>

#define KeyN 3

typedef enum
{
    KEY0 = 0,
    KEY1 = 1,
    KEY2 = 2,
    KEY3 = 3,
} KEY_INDEX;

typedef enum
{
    PULL_UP = 0,
    PULL_DOWN = 1
} KEY_DRIVE;

typedef enum
{
    KEY_CHECK = 0,
    KEY_COMFIRM = 1,
    KEY_RELEASE = 2
} KEY_STATE;

void Key_Init(KEY_INDEX num, int pin, KEY_DRIVE level, uint8_t *key_flag);
int Key_read();
void Key_scan(KEY_INDEX num);
#endif /* __KEY_H_ */