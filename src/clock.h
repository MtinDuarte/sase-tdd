#include "unity.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

/**
 * @brief Enum for function set_TimeOrAlarm to perform
 * set alarm time or set alarm time.  
 * 
 */
enum TIME_ACTIONS
{
    SET_ALARM,
    SET_TIME,
};

/**
 * @brief Callback function type for setting alarm state.
 * 
 */
typedef void (*alarmMatched)(bool);

/**
 * @brief Retrieve the state of the alarm in the system
 * 
 * @return true     The system informs that the alarm time was reached.
 * @return false    The system is not under alarm. 
 */
bool isSystemInAlarm(void);
/**
 * @brief This functions sets how many times will be execute tick functions to reach 1 second. 
 * 
 * @param ticksPerSecond 
 */
void set_TickCount(uint8_t ticksPerSecond);
/**
 * @brief As the requirement needs to have BCD output, this functions recieves 
 *        current the MSB and LSB value of the time format requested, for example
 *        LSB and MSB second value and MSB maximium value 
 *        (in case of seconds 6 >> 60 seconds )  
 *        (in case of minutes 6 >> 60 minutes )
 *        (in case of hours   2 >> 23 hours   )
 * @param high_digit 
 * @param low_digit 
 * @param max_high_digit 
 */
void increment_bcd(uint8_t* high_digit, uint8_t* low_digit, uint8_t max_high_digit);

/**
 * @brief Tick function that increments time units.
 * 
 */
void tick(void);
/**
 * @brief Get the CurrentTime 
 * 
 * @param hour          Pointer to the buffer where the values will be returned.
 * @param arrayLength   Length of the buffer.
 * @return true         The time was returned sucessfuly
 * @return false        The time was not set or is invalid.
 */
bool get_CurrentTime(uint8_t* hour, uint8_t arrayLength);
/**
 * @brief Set the TimeOrAlarm
 * 
 * @param hour          Pointer to the buffer with the time/alarm value to be set.
 * @param arrayLength   Length of the incoming buffer.
 * @param timeAction    Action to be performed could be SET_TIME OR SET_ALARM
 * @return true         Time or alarm correctly set.
 * @return false        Invalid action, buffer length, or null pointer.
 */
bool set_TimeOrAlarm(uint8_t* hour, uint8_t arrayLength, int timeAction);
/**
 * @brief Get the AlarmTime 
 * 
 * @param hour          Pointer to the buffer where the result will be copied.
 * @param arrayLength   Length of the buffer
 * @return true         The alarm time was copied sucessfuly.
 * @return false        The pointer is null or array length invalid.
 */
bool get_AlarmTime(uint8_t* hour, uint8_t arrayLength);
/**
 * @brief Callback function to set the alarm is active.
 * 
 */
void set_AlarmStateCB(bool status);
/**
 * @brief Function to register function that will be used to set alarm status.
 * 
 * @param alarmCB Pointer to callback function
 */
void registerCallback(alarmMatched alarmCB);
/**
 * @brief Set the ShutdownAlarm
 * 
 * @return true     Alarm turned off.
 * @return false    Never returns false.
 */
bool set_ShutdownAlarm(void);
/**
 * @brief Turns off alarm with defined offset.  
 * 
 * @param minutesOffset  Minutes to add for an alarm offset
 * @return true          The offset was sucessfuly added.
 * @return false         The offset was not added.
 */
bool set_ShutdownAlarmWithOffset(uint8_t minutesOffset);
