#include "unity.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "clock.h"

/* Callbacks */

typedef struct 
{
    int         currentTicks;
    int         ticksPerSecond;
    //time [6] = {h,h,m,m,s,s}
    int          time[6];
    int          alarmTime[6];
    int          inAlarm;
    bool         isValid;
    alarmMatched alarmCB;
}test_reloj;

test_reloj g_test_reloj;

/* Callback */
void setAlarmIsActive(void)
{
    g_test_reloj.inAlarm = true;
}

bool isSystemInAlarm(void)
{
    return g_test_reloj.inAlarm;
}

void registerCallback(alarmMatched alarmCB)
{
    g_test_reloj.alarmCB = alarmCB;
}

/* Private functions  */
void set_tickCount(int ticksPerSecond)
{
    g_test_reloj.ticksPerSecond = ticksPerSecond;
}
// Función para incrementar en BCD
void increment_bcd(int* high_digit, int* low_digit, int max_high_digit)
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

void tick(void)
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

    if(g_test_reloj.alarmCB != NULL)
    {
        if(     g_test_reloj.time[0] == g_test_reloj.alarmTime[0]
            &&  g_test_reloj.time[1] == g_test_reloj.alarmTime[1]
            &&  g_test_reloj.time[2] == g_test_reloj.alarmTime[2] 
            &&  g_test_reloj.time[3] == g_test_reloj.alarmTime[3]
            &&  g_test_reloj.time[4] == g_test_reloj.alarmTime[4]
            &&  g_test_reloj.time[5] == g_test_reloj.alarmTime[5])
        {
                g_test_reloj.alarmCB(); 
        }
    }
}

void _tick(void)
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
bool get_CurrentTime(int* hour, int arrayLength)
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
bool set_TimeOrAlarm(int* hour, int arrayLength, int timeAction)
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
bool get_AlarmTime(int* hour, int arrayLength)
{
    memset(hour,0,arrayLength);
    if(hour && arrayLength)
    {
        hour [5] = g_test_reloj.alarmTime[5];
        hour [4] = g_test_reloj.alarmTime[4];
        hour [3] = g_test_reloj.alarmTime[3];
        hour [2] = g_test_reloj.alarmTime[2];
        hour [1] = g_test_reloj.alarmTime[1];
        hour [0] = g_test_reloj.alarmTime[0];
    }
 
}
