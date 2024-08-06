#include "unity.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "clock.h"

#define DEFAULT_TICKS_PER_SECOND 1

// typedef void (*init_callback_t)(void);
// typedef void (*test_callback_t)(void);


void setUp(void);
void test_isTimeInvalid(void);
void test_isTimeSet(void);
void test_verify_5_Tick(void);
void test_verify_120_Tick(void);
//void test_setAlarm(void);

// typedef struct 
// {
//     init_callback_t initFunction;
//     test_callback_t testFunction;
//     bool resultValue;
// }g_testCases;

// g_testCases _testCases [] = 
// {
//     {setUp, test_isTimeInvalid  , NULL},
//     {setUp, test_isTimeSet      , NULL},
//     {setUp, test_verify_5_Tick  , NULL},
//     {setUp, test_verify_120_Tick, NULL},
// };

/* Initialization functions */
void setUp (void)
{
    set_tickCount(DEFAULT_TICKS_PER_SECOND);
}

/* Test cases functions */
void test_isTimeInvalid(void)
{
    int expectedValue [6] = {0,0,0,0,0,0};
    int hour[6] = {0xff};
    int ticksPerSecond = 1;
    set_tickCount(ticksPerSecond);
    TEST_ASSERT_FALSE(get_CurrentTime(hour,sizeof(hour)));
}
void test_isTimeSet(void)
{
    int timeToSet [6] = {1,2,0,0,0,0};
    int hour [6] = {0xff};
    set_TimeOrAlarm(timeToSet,sizeof(timeToSet),SET_TIME);
    TEST_ASSERT_TRUE(get_CurrentTime(hour,sizeof(hour)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(timeToSet,hour,sizeof(hour));    
}
void test_verify_5_Tick(void)
{
    int tickForTest = 5;
    int hour [6] = {0xff};
    
    int expectedValue [6] = {1,2,3,0,0,5};
    int timeToSet  [6] = {1,2,3,0,0,0}; 

    /* Set time to clock */
    set_TimeOrAlarm(timeToSet,sizeof(timeToSet),SET_TIME);

    for (int i = 0 ; i < tickForTest ; i++)
        tick();
    
    /* Get Current time after ticks... */
    get_CurrentTime(hour,sizeof(hour));

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedValue,hour,sizeof(hour));
}
void test_verify_120_Tick(void)
{
    int tickForTest = 60 * 2;
    int hour [6] = {0xff};
    int expectedValue [6] = {1,2,3,2,0,0};
    int timeToSet  [6] = {1,2,3,0,0,0}; 

    /* Set time to clock */
    set_TimeOrAlarm(timeToSet,sizeof(timeToSet),SET_TIME);

    for (int i = 0 ; i < tickForTest ; i++)
        tick();

    TEST_ASSERT_TRUE(get_CurrentTime(hour,sizeof(hour)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedValue,hour,sizeof(hour));
}
void test_setAlarm(void)
{
    int timeToSet     [6] = {1,2,0,0,0,0};
    int alarmToBeSet  [6] = {1,2,0,2,0,0}; 
    int hour          [6] = {0xff};
    int tickForTest       = 130;
    // Setear la hora
    set_TimeOrAlarm(timeToSet,sizeof(timeToSet),SET_TIME);
    set_TimeOrAlarm(alarmToBeSet,sizeof(alarmToBeSet),SET_ALARM);

    for (int i = 0 ; i < tickForTest ; i++)
        tick();
    
    get_AlarmTime(hour,sizeof(hour));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(alarmToBeSet,hour,sizeof(hour));
}
void test_IsSystemInAlarm(void)
{
    int timeToSet     [6] = {1,2,0,0,0,0};
    int alarmToBeSet  [6] = {1,2,0,2,0,0}; 
    int hour          [6] = {0xff};
    int tickForTest       = 60 * 2;
    
    /* Registrar callback */
    registerCallback(setAlarmIsActive);

    /* Setear hora */
    set_TimeOrAlarm(timeToSet,sizeof(timeToSet),SET_TIME);

    /* Setear alarma  */
    set_TimeOrAlarm(alarmToBeSet,sizeof(alarmToBeSet),SET_ALARM);

    for (int i = 0 ; i < tickForTest ; i++)
        tick();
    
    TEST_ASSERT_TRUE(isSystemInAlarm());
}










// void test_reset_clock_overflow(void)
// {
//     int hour [6] = {0xff};

//     static const struct testCases {
//         int timeToSet [6];
//         int expectedValue [6];
//         int tickForTest;
//     }EJEMPLOS[] = {
        
//         { .timeToSet = {1,2,3,0,0,0} , .expectedValue = {1,2,3,0,0,5}, .timeToSet = 1},
//         { .timeToSet = {1,2,3,0,0,0} , .expectedValue = {1,2,3,2,0,0}, .timeToSet = 1},
//         { .timeToSet = {2,3,5,9,5,9},  .expectedValue = {0,0,0,0,0,0}, .timeToSet = 1},
//     };

//     for (int i = 0 ; i < sizeof(EJEMPLOS) / sizeof(struct testCases) ; i++)
//     {
//         memset(hour,0xff,sizeof(hour));

//         set_TimeOrAlarm(&EJEMPLOS[i].timeToSet[0],6,SET_TIME);

//         for (int i = 0 ; i < EJEMPLOS[i].tickForTest ; i++)
//             tick();
        
//         TEST_ASSERT_TRUE(get_CurrentTime(hour,sizeof(hour)));
//         TEST_ASSERT_EQUAL_UINT8_ARRAY(&EJEMPLOS[i].expectedValue[0],hour,sizeof(hour));
//     }
// }