#include "camera_control.h"
#include <assert.h>

#define LED_RED         (1U << 1U)
#define PIN_SHUTTER     (1 << 1U) /*PF1*/
#define LED_BLUE        (1U << 2U)
#define LED_GREEN       (1U << 3U)
#define PIN_AUTOFOCUS    (1U << 3U) /*PF3*/
#define DEACTIVATE_OUTPUTS (0x00)
#define SHUTTER_DELAY_MS (32000)
#define ONE_SECOND_TIMER_VALUE (0x00F42400) /*16Mhz, one second*/

static bool isShootProcessRunning = false;
static bool isTimeForShooter = false;
static bool pictureTaken = false;

static inline void processModeContinuousFocus(sensorParameters_t* sensorParametersPtr);
static inline void processModeFocusAndShoot(sensorParameters_t* sensorParametersPtr);
static inline void shootPicture(void);
static inline void resetTimer(void);
static inline bool delayPressingShutterReached(void);
static inline void activatePin(uint8_t pin);
static inline void deactivatePin(uint8_t pin);
static inline void timerDelayDueToNumberOfPicture(sensorParameters_t* sensorParametersPtr);

void TIMER0_Handler(void)
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

/* Intern functions */

static inline void processModeContinuousFocus(sensorParameters_t* sensorParametersPtr)
{
    activatePin(PIN_AUTOFOCUS);

    if(sensorParametersPtr->isInputTrigered || isShootProcessRunning)
    {
        isShootProcessRunning = true;

        if(sensorParametersPtr->isFirstPictureToBeTaken)
        {
            shootPicture();
        }
        else
        {
            TIMER0_CTL_R |= (1 << 0); /* Enables TIMER 0 */

            if(isTimeForShooter)
            {
                shootPicture();
            }
        }
    }
    else
    {
        resetTimer();
        deactivatePin(PIN_SHUTTER);
    }
}

static inline void processModeFocusAndShoot(sensorParameters_t* sensorParametersPtr)
{
    if(sensorParametersPtr->isInputTrigered || isShootProcessRunning)
    {
        isShootProcessRunning = true;
        timerDelayDueToNumberOfPicture(sensorParametersPtr);
        TIMER0_CTL_R |= (1 << 0); /* Enables TIMER 0 */
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

static inline void shootPicture(void)
{
    if(delayPressingShutterReached())
    {
        isTimeForShooter = false;
        isShootProcessRunning = false;
        pictureTaken = true;
    }
    else
    {
        activatePin(PIN_SHUTTER);
    }
}

static inline void timerDelayDueToNumberOfPicture(sensorParameters_t* sensorParametersPtr)
{
    if(sensorParametersPtr->isFirstPictureToBeTaken && sensorParametersPtr->isInputTrigered)
    {
        TIMER0_TAILR_R = 4 * ONE_SECOND_TIMER_VALUE;
    }
    else if(sensorParametersPtr->isInputTrigered)
    {
        TIMER0_TAILR_R = ONE_SECOND_TIMER_VALUE;
    }
}

static inline bool delayPressingShutterReached(void)
{
    static uint16_t ledTimeCounter = 0;
    
    ledTimeCounter++;

    if(ledTimeCounter >= SHUTTER_DELAY_MS)
    {
        ledTimeCounter = 0;
        return true;
    }

    return false;
}

static inline void activatePin(uint8_t pin)
{
    GPIOF_HS->DATA_Bits[pin] = 0xFFFFFFFF;
}

static inline void deactivatePin(uint8_t pin)
{
    GPIOF_HS->DATA_Bits[pin] = 0;
}

static inline void resetTimer(void)
{
    TIMER0_CTL_R &= ~(1 << 0); /* Disables TIMER A */
    TIMER0_TAILR_R = ONE_SECOND_TIMER_VALUE; 
    TIMER0_ICR_R |= (1 << 0); /* Writing a 1 to this bit clears the TATORIS bit */
} 

bool isPictureTaken(void)
{
    if(pictureTaken)
    {
        pictureTaken = false;
        return true;
    }

    return false;
}
