#include "unity.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

typedef void (*alarmMatched)(void);

enum TIME_ACTIONS
{
    SET_ALARM,
    SET_TIME,
};
bool isSystemInAlarm(void);
void set_tickCount(int ticksPerSecond);
void increment_bcd(int* high_digit, int* low_digit, int max_high_digit);
void tick(void);
bool get_CurrentTime(int* hour, int arrayLength);
bool set_TimeOrAlarm(int* hour, int arrayLength, int timeAction);
bool get_AlarmTime(int* hour, int arrayLength);
void setAlarmIsActive(void);
void registerCallback(alarmMatched alarmCB);
