#include "camera_control.h"

#define LED_RED (1U << 1U)
#define PIN_SHUTTER (1 << 1U)
#define LED_BLUE (1U << 2U)
#define LED_GREEN (1U << 3U)
#define PIN_AUTOFOCUS (1U << 3U)
#define DEACTIVATE_OUTPUTS (0x00)
#define SHUTTER_DELAY (32000)
#define ONE_SECOND_TIMER (0x00F42400) /*16Mhz, one second*/

static bool flag_led = false;

static bool isTimeForShutter = false;

inline static void activateAutoFocusTimerAndSetFlag(void);
inline static void deactivateFlagFromTimers(void);
inline static void resetTimer(void);
inline static bool delayForShutterReached(void);
inline static void activatePin(uint8_t pin);


void TIMER0_Handler( void )
{
    TIMER0->ICR |= 1;
    isTimeForShutter = true;
}

void camera_control(bool isInputTriggered)
{
    if(isInputTriggered || flag_led)
    {
        activateAutoFocusTimerAndSetFlag();
        activatePin(PIN_AUTOFOCUS);
        if(isTimeForShutter)
        {
            activatePin(PIN_SHUTTER);
            if(delayForShutterReached())
            {
                deactivateFlagFromTimers();
            }
        }
    }
    else
    {
        resetTimer();
        activatePin(DEACTIVATE_OUTPUTS);
    }
}

/*----------------Intern functions-----------------------------*/

inline static bool delayForShutterReached(void)
{
    bool retVal = false;
    static uint16_t led_time_counter = 0;
    
    led_time_counter ++;
    if(led_time_counter >= SHUTTER_DELAY)
    {
        led_time_counter = 0;
        retVal = true;
    }
    return retVal;
}

inline static void activatePin(uint8_t pin)
{
    GPIOF_HS->DATA_Bits[PIN_SHUTTER | PIN_AUTOFOCUS] = pin;
}

inline static void activateAutoFocusTimerAndSetFlag(void)
{
    flag_led = true;
    TIMER0_CTL_R |= (1 << 0);
}

inline static void deactivateFlagFromTimers(void)
{
    isTimeForShutter = false;
    flag_led = false;
}

inline static void resetTimer(void)
{
    TIMER0_CTL_R &= ~(1 << 0);
    TIMER0_TAILR_R = ONE_SECOND_TIMER; 
    TIMER0_ICR_R |= (1 << 0);
}