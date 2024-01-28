#include "CameraControl.h"

#include <assert.h>

#include "HAL/GPIO/GpioPortF.h"
#include "HAL/TIMER/Timer0.h"

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

static void processModeContinuousFocus(sensorParameters_t* sensorParametersPtr);
static void processModeFocusAndShoot(sensorParameters_t* sensorParametersPtr);
static void shootPicture(void);
static void resetTimer(void);
static bool delayPressingShutterReached(void);
static void timerDelayDueToNumberOfPicture(sensorParameters_t* sensorParametersPtr);

void TIMER0_Handler(void)
{
    Timer0_clearInterrupt();
    isTimeForShooter = true;
}

void CameraControl_Control(sensorParameters_t* sensorParametersPtr)
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

bool CameraControl_IsPictureTaken(void)
{
    if(pictureTaken)
    {
        pictureTaken = false;
        return true;
    }

    return false;
}

/* ----------------Intern functions--------------------------------------------- */

static void processModeContinuousFocus(sensorParameters_t* sensorParametersPtr)
{
    GpioPortF_activatePin(PIN_AUTOFOCUS);

    if(sensorParametersPtr->isInputTrigered || isShootProcessRunning)
    {
        isShootProcessRunning = true;

        if(sensorParametersPtr->isFirstPictureToBeTaken)
        {
            shootPicture();
        }
        else
        {
            Timer0_start();

            if(isTimeForShooter)
            {
                shootPicture();
            }
        }
    }
    else
    {
        resetTimer();
        GpioPortF_deactivatePin(PIN_SHUTTER);
    }
}

static void processModeFocusAndShoot(sensorParameters_t* sensorParametersPtr)
{
    if(sensorParametersPtr->isInputTrigered || isShootProcessRunning)
    {
        isShootProcessRunning = true;
        timerDelayDueToNumberOfPicture(sensorParametersPtr);
        Timer0_start();
        GpioPortF_activatePin(PIN_AUTOFOCUS);

        if(isTimeForShooter)
        {
            shootPicture();
        }
    }
    else
    {
        resetTimer();
        GpioPortF_deactivatePin(PIN_AUTOFOCUS | PIN_SHUTTER);
    }
}

static void shootPicture(void)
{
    if(delayPressingShutterReached())
    {
        isTimeForShooter = false;
        isShootProcessRunning = false;
        pictureTaken = true;
    }
    else
    {
        GpioPortF_activatePin(PIN_AUTOFOCUS | PIN_SHUTTER);
    }
}

static void timerDelayDueToNumberOfPicture(sensorParameters_t* sensorParametersPtr)
{
    if(sensorParametersPtr->isFirstPictureToBeTaken && sensorParametersPtr->isInputTrigered)
    {
        Timer0_setTimerCounter(4 * ONE_SECOND_TIMER_VALUE);
    }
    else if(sensorParametersPtr->isInputTrigered)
    {
        Timer0_setTimerCounter(ONE_SECOND_TIMER_VALUE);
    }
}

static bool delayPressingShutterReached(void)
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

static void resetTimer(void)
{
    Timer0_stop();
    Timer0_setTimerCounter(ONE_SECOND_TIMER_VALUE);
    Timer0_clearInterrupt();
} 


