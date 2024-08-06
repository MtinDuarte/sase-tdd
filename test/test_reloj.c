#include "unity.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "clock.h"

#define DEFAULT_TICKS_PER_SECOND 1
#define ONE_MINUTE_IN_SECONDS    60
#define DEFAULT_ARRAY_LENGTH     6
#define ARRAY_DEFAULT_VALUE      0xff
#define START_TICKS(ticks) for (int i = 0 ; i < tickForTest ; i++) tick(); 

int g_arrayExpected [DEFAULT_ARRAY_LENGTH];
int g_currentHour   [DEFAULT_ARRAY_LENGTH];

/* Initialization functions */
void setUp (void)
{
    memset(g_arrayExpected,0,DEFAULT_ARRAY_LENGTH);
    memset(g_currentHour,ARRAY_DEFAULT_VALUE,DEFAULT_ARRAY_LENGTH);
    set_tickCount(DEFAULT_TICKS_PER_SECOND);
}

/* Test cases functions */
void test_isTimeInvalid(void)
{
    TEST_ASSERT_FALSE(get_CurrentTime(g_currentHour,sizeof(g_currentHour)));
}
void test_isTimeSet(void)
{
    int timeToSet [DEFAULT_ARRAY_LENGTH] = {1,2,0,0,0,0};
 
    set_TimeOrAlarm(timeToSet,sizeof(timeToSet),SET_TIME);

    TEST_ASSERT_TRUE(get_CurrentTime(g_currentHour,sizeof(g_currentHour)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(timeToSet,g_currentHour,sizeof(g_currentHour));    
}
void test_verify_5_Tick(void)
{
    int tickForTest                          = 5;
    int expectedValue [DEFAULT_ARRAY_LENGTH] = {1,2,3,0,0,5};
    int timeToSet     [DEFAULT_ARRAY_LENGTH] = {1,2,3,0,0,0}; 

    /* Set time to clock */
    set_TimeOrAlarm(timeToSet,sizeof(timeToSet),SET_TIME);

    START_TICKS(tickForTest);
    
    /* Get Current time after ticks... */
    TEST_ASSERT_TRUE(get_CurrentTime(g_currentHour,sizeof(g_currentHour)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedValue,g_currentHour,sizeof(g_currentHour));
}
void test_verify_120_Tick(void)
{
    int tickForTest = ONE_MINUTE_IN_SECONDS * 2;
    int expectedValue   [DEFAULT_ARRAY_LENGTH] = {1,2,3,2,0,0};
    int timeToSet       [DEFAULT_ARRAY_LENGTH] = {1,2,3,0,0,0}; 

    /* Set time to clock */
    set_TimeOrAlarm(timeToSet,sizeof(timeToSet),SET_TIME);

    START_TICKS(tickForTest);

    TEST_ASSERT_TRUE(get_CurrentTime(g_currentHour,sizeof(g_currentHour)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedValue,g_currentHour,sizeof(g_currentHour));
}
void test_setAlarm(void)
{
    int timeToSet     [DEFAULT_ARRAY_LENGTH] = {1,2,0,0,0,0};
    int alarmToBeSet  [DEFAULT_ARRAY_LENGTH] = {1,2,0,2,0,0}; 
    int tickForTest       = 130;
    // Setear la hora
    set_TimeOrAlarm(timeToSet,sizeof(timeToSet),SET_TIME);
    set_TimeOrAlarm(alarmToBeSet,sizeof(alarmToBeSet),SET_ALARM);

    START_TICKS(tickForTest)
    
    get_AlarmTime(g_currentHour,sizeof(g_currentHour));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(alarmToBeSet,g_currentHour,sizeof(g_currentHour));
}
void test_IsSystemInAlarm(void)
{
    int timeToSet     [DEFAULT_ARRAY_LENGTH] = {1,2,0,0,0,0};
    int alarmToBeSet  [DEFAULT_ARRAY_LENGTH] = {1,2,0,2,0,0}; 
    int tickForTest       = ONE_MINUTE_IN_SECONDS * 2;
    
    /* Registrar callback */
    registerCallback(setAlarmIsActive);

    /* Setear hora */
    set_TimeOrAlarm(timeToSet,sizeof(timeToSet),SET_TIME);

    /* Setear alarma  */
    set_TimeOrAlarm(alarmToBeSet,sizeof(alarmToBeSet),SET_ALARM);

    START_TICKS(tickForTest);
    
    TEST_ASSERT_TRUE(isSystemInAlarm());
}
void test_shutdownAlarm(void)
{
    int timeToSet     [DEFAULT_ARRAY_LENGTH] = {1,2,0,0,0,0};
    int alarmToBeSet  [DEFAULT_ARRAY_LENGTH] = {1,2,0,2,0,0}; 
    int hour          [DEFAULT_ARRAY_LENGTH] = { ARRAY_DEFAULT_VALUE };
    int tickForTest       = ONE_MINUTE_IN_SECONDS * 3;
    
    /* Registrar callback */
    registerCallback(setAlarmIsActive);

    /* Setear hora */
    set_TimeOrAlarm(timeToSet,sizeof(timeToSet),SET_TIME);

    /* Setear alarma  */
    set_TimeOrAlarm(alarmToBeSet,sizeof(alarmToBeSet),SET_ALARM);

    START_TICKS(tickForTest);
    
    TEST_ASSERT_TRUE(isSystemInAlarm());

    set_ShutdownAlarm();

    START_TICKS(tickForTest);

    TEST_ASSERT_FALSE(isSystemInAlarm());
}
void test_shutdownAlarmWithOffset(void)
{
    // Offset de alarma 00:02:00
    int alarmOffsetMinutes = 2;

    /* Registrar callback */
    registerCallback(setAlarmIsActive);

    // Seteo 12:00:00
    int timeToSet     [DEFAULT_ARRAY_LENGTH] = {1,2,0,0,0,0};
    /* Setear hora */
    set_TimeOrAlarm(timeToSet,sizeof(timeToSet),SET_TIME);

    // Hora de alarma 12:02:00
    int alarmToBeSet  [DEFAULT_ARRAY_LENGTH] = {1,2,0,2,0,0}; 
    
    /* Setear alarma  */
    set_TimeOrAlarm(alarmToBeSet,sizeof(alarmToBeSet),SET_ALARM);

    int tickForTest       = ONE_MINUTE_IN_SECONDS;
    
    // Pasa 1 MINUTO : No debe estar en alarma
    START_TICKS(tickForTest); TEST_ASSERT_FALSE(isSystemInAlarm());
    // Pasa 1 MINUTO : Debe entrar en alarma
    START_TICKS(tickForTest); TEST_ASSERT_TRUE(isSystemInAlarm());

    // Apagar alarma con offset de 2 minutos
     set_ShutdownAlarmWithOffset(2);

    START_TICKS(tickForTest);TEST_ASSERT_FALSE(isSystemInAlarm());
    START_TICKS(tickForTest);TEST_ASSERT_TRUE(isSystemInAlarm());
    
    // Apago alarma por completo
    set_ShutdownAlarm(); TEST_ASSERT_FALSE(isSystemInAlarm());
}






// typedef void (*init_callback_t)(void);
// typedef void (*test_callback_t)(void);

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