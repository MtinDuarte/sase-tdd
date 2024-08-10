#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "clock.h"

/* Callbacks */

typedef struct 
{
    alarmMatched alarmCB;
    bool         isAlarmActive;
    bool         inAlarm;
    bool         isValid;
    uint8_t      alarmOffset;
    uint8_t      alarmTime[6];  // {h,h,m,m,s,s}
    uint8_t      time[6];       // {h,h,m,m,s,s}    
    uint32_t     currentTicks;
    uint32_t     ticksPerSecond;
}test_reloj;

test_reloj g_test_reloj;

/**
 * @brief Set the Alarm Is Active
 * 
 */
void set_AlarmStateCB(bool status)
{
    g_test_reloj.inAlarm = status;
}
bool isSystemInAlarm(void)
{
    return g_test_reloj.inAlarm;
}
void registerCallback(alarmMatched alarmCB)
{
    g_test_reloj.alarmCB = alarmCB;
}
// Función para incrementar en BCD
void increment_bcd(uint8_t* high_digit, uint8_t* low_digit, uint8_t max_high_digit)
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
            // digito mas significativo alcanzaron máximo valor.
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
                g_test_reloj.alarmCB(true); 
        }
    }
}



/* Setters */
void set_TickCount(uint8_t ticksPerSecond)
{
    g_test_reloj.ticksPerSecond = ticksPerSecond;
}
bool set_ShutdownAlarm(void)
{
    g_test_reloj.inAlarm = false;
}
bool set_TimeOrAlarm(uint8_t* hour, uint8_t arrayLength, int timeAction)
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
/**
 * @brief Set the ShutdownAlarmWithOffset
 * 
 * @param minutesOffset     Offset for the alarm synced with the current time.
 * @return true             Alarm values correctly set.
 */
bool set_ShutdownAlarmWithOffset(uint8_t minutesOffset)
{
    /*  Sync alarmTime with the current time  + offset */
    g_test_reloj.alarmTime [0] = g_test_reloj.time[0];
    g_test_reloj.alarmTime [1] = g_test_reloj.time[1];
    g_test_reloj.alarmTime [2] = g_test_reloj.time[2];
    g_test_reloj.alarmTime [3] = g_test_reloj.time[3];
    g_test_reloj.alarmTime [4] = g_test_reloj.time[4];
    g_test_reloj.alarmTime [5] = g_test_reloj.time[5];

    for (int i = 0; i < minutesOffset; i++) 
    {
        increment_bcd(&g_test_reloj.alarmTime[2], &g_test_reloj.alarmTime[3], 6); // Incrementa los minutos

        // Si los minutos se reinician a 00, incrementa las horas
        if (g_test_reloj.alarmTime[2] == 0 && g_test_reloj.alarmTime[3] == 0) 
        {
            increment_bcd(&g_test_reloj.alarmTime[0], &g_test_reloj.alarmTime[1], 3); // Incrementa las horas

            // Ajustar para 24 horas en BCD (00-23)
            if (g_test_reloj.alarmTime[0] == 2 && g_test_reloj.alarmTime[1] >= 4) {
                g_test_reloj.alarmTime[0] = 0;
                g_test_reloj.alarmTime[1] = 0;
            }
        }
    }
    g_test_reloj.inAlarm = false;
    return true;
}


/**
 * @brief Get the CurrentTime object
 * 
 * @param hour          The pointer to the buffer to store time values
 * @param arrayLength   The length of the buffer where the values will be copied.
 * @return true         In case of time is valid (previouly set) and copied sucessfuly
 * @return false        Time was never set and pointer to buffer will return all zeros.
 */
bool get_CurrentTime(uint8_t* hourBuffer, uint8_t arrayLength)
{
    /* Obtener el valor de retorno. Obtener la hora correcta / incorrecta.  */
    memset(hourBuffer,0,arrayLength);

    if(g_test_reloj.isValid && hourBuffer)
    {
        // hour [6] = {h,h,m,m,s,s}
        hourBuffer[5] =  g_test_reloj.time[5];
        hourBuffer[4] =  g_test_reloj.time[4];
        hourBuffer[3] =  g_test_reloj.time[3];
        hourBuffer[2] =  g_test_reloj.time[2];
        hourBuffer[1] =  g_test_reloj.time[1];
        hourBuffer[0] =  g_test_reloj.time[0];        
        return true;
    }
    return false;
}
/**
 * @brief Get the AlarmTime object
 * 
 * @param hourBuffer    Pointer to the buffer where the data will be copied.
 * @param arrayLength   Array length of the pointer where the values will be copied.
 * @return true         AlarmTime was successfuly copied in HourBuffer
 * @return false 
 */
bool get_AlarmTime(uint8_t* hourBuffer, uint8_t arrayLength)
{
    memset(hourBuffer,0,arrayLength);
    if(hourBuffer && arrayLength)
    {
        hourBuffer [5] = g_test_reloj.alarmTime[5];
        hourBuffer [4] = g_test_reloj.alarmTime[4];
        hourBuffer [3] = g_test_reloj.alarmTime[3];
        hourBuffer [2] = g_test_reloj.alarmTime[2];
        hourBuffer [1] = g_test_reloj.alarmTime[1];
        hourBuffer [0] = g_test_reloj.alarmTime[0];
        return true;
    }
    return false;
}

