#include "camera_control.h"

#define LED_RED (1U << 1U)
#define LED_BLUE (1U << 2U)
#define LED_GREEN (1U << 3U)
#define NO_COLOR (0x00)
#define RED_LED_DELAY (32000)
#define ONE_SECOND_TIMER (0x00F42400) /*16Mhz, one second*/

static bool flag_led = false;

static bool isTimer0Activated = false;

inline static void activateTimerAndSetFlag(void);
inline static void deactivateFlagFromTimers(void);
inline static void resetTimer(void);
inline static bool delayForRedLedReached(void);
inline static void activateLed(uint8_t ledColor);


void TIMER0_Handler( void )
{
    TIMER0->ICR |= 1;
    isTimer0Activated = true;
}

void camera_control(bool isInputTriggered)
{
    if(isInputTriggered || flag_led)
    {
        activateTimerAndSetFlag();
        activateLed(LED_GREEN);
        if(isTimer0Activated)
        {
            activateLed(LED_RED);
            if(delayForRedLedReached())
            {
                deactivateFlagFromTimers();
            }
        }
    }
    else
    {
        resetTimer();
        activateLed(NO_COLOR);
    }
}

/*----------------Intern functions-----------------------------*/

inline static bool delayForRedLedReached(void)
{
    bool retVal = false;
    static uint16_t led_time_counter = 0;
    
    led_time_counter ++;
    if(led_time_counter >= RED_LED_DELAY)
    {
        led_time_counter = 0;
        retVal = true;
    }
    return retVal;
}

inline static void activateLed(uint8_t ledColor)
{
    GPIO_PORTF_AHB_DATA_BITS_R[LED_RED | LED_BLUE | LED_GREEN] = ledColor;
}

inline static void activateTimerAndSetFlag(void)
{
    flag_led = true;
    TIMER0_CTL_R |= (1 << 0);
}

inline static void deactivateFlagFromTimers(void)
{
    isTimer0Activated = false;
    flag_led = false;
}

inline static void resetTimer(void)
{
    TIMER0_CTL_R &= ~(1 << 0);
    TIMER0_TAILR_R = ONE_SECOND_TIMER; 
    TIMER0_ICR_R |= (1 << 0);
}