#include "unity.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#define DEFAULT_TICKS_PER_SECOND 1

enum TIME_ACTIONS
{
    SET_ALARM,
    SET_TIME,
};

typedef void (*init_callback_t)(void);
typedef void (*test_callback_t)(void);
typedef void (*alarmMatched)(void);

void setUp(void);
void test_isTimeInvalid(void);
void test_isTimeSet(void);
void test_verify_5_Tick(void);
void test_verify_120_Tick(void);

typedef struct 
{
    init_callback_t initFunction;
    test_callback_t testFunction;
    bool resultValue;
}g_testCases;

g_testCases _testCases [] = 
{
    {setUp, test_isTimeInvalid  , NULL},
    {setUp, test_isTimeSet      , NULL},
    {setUp, test_verify_5_Tick  , NULL},
    {setUp, test_verify_120_Tick, NULL},
};

typedef struct 
{
    int         currentTicks;
    int         ticksPerSecond;
    //time [6] = {h,h,m,m,s,s}
    int          time[6];
    int          alarmTime[6];
    int          inAlarm;
    bool         isValid;
    alarmMatched alarmMatch;
}test_reloj;

test_reloj g_test_reloj;

void setAlarmIsActive(void)
{
    g_test_reloj.inAlarm = true;
}

/* Initialization functions */
void setUp (void)
{
    g_test_reloj.alarmMatch = setAlarmIsActive;
    memset(g_test_reloj.alarmTime,0,sizeof(g_test_reloj.alarmTime));
    memset(g_test_reloj.time,0,sizeof(g_test_reloj.alarmTime));
    g_test_reloj.isValid = false;
    g_test_reloj.inAlarm = false;
    g_test_reloj.currentTicks = 0;
    g_test_reloj.ticksPerSecond = DEFAULT_TICKS_PER_SECOND;
}

/* Private functions  */
static void set_tickCount(int ticksPerSecond)
{
    g_test_reloj.ticksPerSecond = ticksPerSecond;
}
// Función para incrementar en BCD
static void increment_bcd(int* high_digit, int* low_digit, int max_high_digit)
{
    // Incremento digito menos significativo
    (*low_digit)++;

    if (*low_digit >= 10)  
    {
        // Segundos == 10, resetear e incrementar el próximo valor
        *low_digit = 0;

        (*high_digit)++;

        // Para las horas el máximo es 23, y para los minutos y segundos tengo 60 como máximo.
        if (*high_digit >= max_high_digit) 
        {
            // digíto más significativo alcanzaron máximo valor.
            *high_digit = 0;
        }
    }
}

static void tick(void)
{
    g_test_reloj.currentTicks++;

    if (g_test_reloj.currentTicks >= g_test_reloj.ticksPerSecond) 
    {
        g_test_reloj.currentTicks = 0;

        // Incrementar los segundos: "hh-mm-ss" posiciones 4 y 5 del array.
        increment_bcd(&g_test_reloj.time[4], &g_test_reloj.time[5], 6);

        // Incrementar los minutos si es necesario
        if (g_test_reloj.time[4] == 0 && 
            g_test_reloj.time[5] == 0) 
        {
            increment_bcd(&g_test_reloj.time[2], &g_test_reloj.time[3], 6);
        }
        // Incrementar las horas: si minutos y segundos son cero
        if (g_test_reloj.time[2] == 0 && 
            g_test_reloj.time[3] == 0 && 
            g_test_reloj.time[4] == 0 && 
            g_test_reloj.time[5] == 0)
        {
            increment_bcd(&g_test_reloj.time[0], &g_test_reloj.time[1], 3);
            
            // Ajustar para 24 horas en BCD (00-23)
            if (g_test_reloj.time[0] == 2 && 
                g_test_reloj.time[1] >= 4) 
            {
                g_test_reloj.time[0] = 0;
                g_test_reloj.time[1] = 0;
            }
        }
    }

    if(g_test_reloj.time[0] == g_test_reloj.alarmTime[0] &&
       g_test_reloj.time[1] == g_test_reloj.alarmTime[1] &&
       g_test_reloj.time[2] == g_test_reloj.alarmTime[2] &&
       g_test_reloj.time[3] == g_test_reloj.alarmTime[3] &&
       g_test_reloj.time[4] == g_test_reloj.alarmTime[4] &&
       g_test_reloj.time[5] == g_test_reloj.alarmTime[5])
       {
            g_test_reloj.alarmMatch(); 
       }
}

static void _tick(void)
{
    // Incrementa los ticks actuales
    g_test_reloj.currentTicks++;

    // Comprueba si los ticks actuales han alcanzado los ticks por segundo
    if (g_test_reloj.currentTicks >= g_test_reloj.ticksPerSecond) 
    {
        // Resetear los ticks actuales
        g_test_reloj.currentTicks = 0;

        // Incrementar los segundos
        g_test_reloj.time[5]++;
        if (g_test_reloj.time[5] >= 10) {
            g_test_reloj.time[5] = 0;
            g_test_reloj.time[4]++;
            if (g_test_reloj.time[4] >= 6) {
                g_test_reloj.time[4] = 0;

                // Incrementar los minutos
                g_test_reloj.time[3]++;
                if (g_test_reloj.time[3] >= 10) {
                    g_test_reloj.time[3] = 0;
                    g_test_reloj.time[2]++;
                    if (g_test_reloj.time[2] >= 6) {
                        g_test_reloj.time[2] = 0;

                        // Incrementar las horas
                        g_test_reloj.time[1]++;
                        if (g_test_reloj.time[1] >= 10) {
                            g_test_reloj.time[1] = 0;
                            g_test_reloj.time[0]++;
                            if (g_test_reloj.time[0] >= 2 && g_test_reloj.time[1] >= 4) {
                                // Resetear las horas
                                g_test_reloj.time[0] = 0;
                                g_test_reloj.time[1] = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}
static bool get_CurrentTime(int* hour, int arrayLength)
{
    /* Obtener el valor de retorno. Obtener la hora correcta / incorrecta.  */
    memset(hour,0,arrayLength);

    if(g_test_reloj.isValid)
    {
        // hour [6] = {h,h,m,m,s,s}
        hour[5] =  g_test_reloj.time[5];
        hour[4] =  g_test_reloj.time[4];
        hour[3] =  g_test_reloj.time[3];
        hour[2] =  g_test_reloj.time[2];
        hour[1] =  g_test_reloj.time[1];
        hour[0] =  g_test_reloj.time[0];        
        return true;
    }
    return false;
}
static bool set_TimeOrAlarm(int* hour, int arrayLength, int timeAction)
{
    if(timeAction == SET_TIME && hour && arrayLength)
    {
        g_test_reloj.time[5] = hour [5];
        g_test_reloj.time[4] = hour [4];
        g_test_reloj.time[3] = hour [3];
        g_test_reloj.time[2] = hour [2];
        g_test_reloj.time[1] = hour [1];
        g_test_reloj.time[0] = hour [0];
    }else if (timeAction == SET_ALARM && hour && arrayLength)
    {
        g_test_reloj.alarmTime[5] = hour [5];
        g_test_reloj.alarmTime[4] = hour [4];
        g_test_reloj.alarmTime[3] = hour [3];
        g_test_reloj.alarmTime[2] = hour [2];
        g_test_reloj.alarmTime[1] = hour [1];
        g_test_reloj.alarmTime[0] = hour [0];
    }else
    {
        g_test_reloj.isValid = false;
        return false;        
    }
    g_test_reloj.isValid = true; 
    return true;
}
static bool get_AlarmTime(int* hour, int arrayLength)
{
    memset(hour,0,arrayLength);
    hour [5]= g_test_reloj.alarmTime[5];
    hour [4]= g_test_reloj.alarmTime[4];
    hour [3]= g_test_reloj.alarmTime[3];
    hour [2]= g_test_reloj.alarmTime[2];
    hour [1]= g_test_reloj.alarmTime[1];
    hour [0]= g_test_reloj.alarmTime[0]; 
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
    int hour [6] = {0x22};
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
    int tickForTest = 120;
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
    int tickForTest       = 2*60 + 10;
    // Setear la hora
    set_TimeOrAlarm(timeToSet,sizeof(timeToSet),SET_TIME);
    set_TimeOrAlarm(alarmToBeSet,sizeof(alarmToBeSet),SET_ALARM);

    for (int i = 0 ; i < tickForTest ; i++)
        tick();
    
    TEST_ASSERT_TRUE(g_test_reloj.inAlarm == true);
    //TEST_ASSERT_EQUAL_UINT8_ARRAY(alarmToBeSet,hour,sizeof(hour));
}



