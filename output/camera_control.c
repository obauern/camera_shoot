#include "camera_control.h"
#include <assert.h>

#define LED_RED (1U << 1U)
#define PIN_SHUTTER (1 << 1U) /*PF1*/
#define LED_BLUE (1U << 2U)
#define LED_GREEN (1U << 3U)
#define PIN_AUTOFOCUS (1U << 3U) /*PF3*/
#define DEACTIVATE_OUTPUTS (0x00)
#define SHUTTER_DELAY (32000)
#define ONE_SECOND_TIMER (0x00F42400) /*16Mhz, one second*/

static bool isShootProcessRunning = false; /*Used to be in the shoot condition until the
                                picture is taken*/

static bool isTimeForShooter = false;
static bool pictureTaked = false;

inline static void processModeFocusAndShoot(sensorParameters_t* sensorParametersPtr);
inline static void processModeContinuousFocus(sensorParameters_t* sensorParametersPtr);
inline static void shootPicture(void);
inline static void resetTimer(void);
inline static bool delayPressingShutterReached(void);
inline static void activatePin(uint8_t pin);
inline static void deactivatePin(uint8_t pin);
inline static void timerDelayDueToNumberOfPicture(sensorParameters_t* sensorParametersPtr);

void TIMER0_Handler( void )
{
    TIMER0->ICR |= 1; /* Writing a 1 to this bit clears the TATORIS bit */
    isTimeForShooter = true;
}

void camera_control(sensorParameters_t* sensorParametersPtr)
{
  switch(sensorParametersPtr->shootMode)
  {
  case MODE_FOCUS_AND_SHOOT:
        processModeFocusAndShoot(sensorParametersPtr);
    break;
  case MODE_CONTINUOUSFOCUS_AND_SHOOT:
        processModeContinuousFocus(sensorParametersPtr);
    break;
  default:
    assert(false);
    break;
  }
}

/*----------------Intern functions-----------------------------*/

inline static void processModeContinuousFocus(sensorParameters_t* sensorParametersPtr)
{
    activatePin(PIN_AUTOFOCUS); /*On this mode the autofocus will allways be set*/
    if(sensorParametersPtr->isInputTrigered || isShootProcessRunning)
    {
        isShootProcessRunning = true;
        if(true == sensorParametersPtr->isFirstPictureToBeTaken)
        {
            shootPicture();
            TIMER0_CTL_R |= (1 << 0); /*Enables TIMER 0*/
        }
        if(isTimeForShooter)
        {
            shootPicture();
        }
    }
    else
    {
        resetTimer();
        deactivatePin(PIN_SHUTTER);
    }
}

inline static void processModeFocusAndShoot(sensorParameters_t* sensorParametersPtr)
{
    if(sensorParametersPtr->isInputTrigered || isShootProcessRunning)
    {
        isShootProcessRunning = true;
        timerDelayDueToNumberOfPicture(sensorParametersPtr);
        TIMER0_CTL_R |= (1 << 0); /*Enables TIMER 0*/
        activatePin(PIN_AUTOFOCUS);
        if(isTimeForShooter)
        {
            shootPicture();
        }
    }
    else
    {
        resetTimer();
        activatePin(PIN_AUTOFOCUS | PIN_SHUTTER);
    }
}

inline static void shootPicture(void)
{
    if(delayPressingShutterReached())
    {
        isTimeForShooter = false;
        isShootProcessRunning = false;
        pictureTaked = true;
    }
    else
    {
        activatePin(PIN_SHUTTER);
    }
}

inline static void timerDelayDueToNumberOfPicture(sensorParameters_t* sensorParametersPtr)
{
    if(sensorParametersPtr->isFirstPictureToBeTaken && sensorParametersPtr->isInputTrigered)
    {
        TIMER0_TAILR_R = 4*ONE_SECOND_TIMER;
    }
    else if(sensorParametersPtr->isInputTrigered)
    {
        TIMER0_TAILR_R = ONE_SECOND_TIMER;
    }
    return;
}

inline static bool delayPressingShutterReached(void)
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
    GPIOF_HS->DATA_Bits[pin] = 0xFFFFFFFF;
}

inline static void deactivatePin(uint8_t pin)
{
    GPIOF_HS->DATA_Bits[pin] = 0;
}

inline static void resetTimer(void)
{
    TIMER0_CTL_R &= ~(1 << 0); /*Disables TIMER A*/
    TIMER0_TAILR_R = ONE_SECOND_TIMER; 
    TIMER0_ICR_R |= (1 << 0); /* Writing a 1 to this bit clears the TATORIS bit */
} 

bool isPictureTaked(void)
{
    if(pictureTaked)
    {
        pictureTaked = false;
        return true;
    }
    return false;
}