#include "Application/Camera/output/CameraControl.h"

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

typedef struct cameraControl_Tag
{
    CameraControlMode_t cameraControlMode;
    bool triggerManualPicture;
}cameraControl_t;

static cameraControl_t cameraControl;

static void handleSensorCameraControl(void);
static void processModeContinuousFocus(sensorParameters_t sensorParameters);
static void processModeFocusAndShoot(sensorParameters_t sensorParameters);
static void shootPicture(void);
static void resetTimer(void);
static bool delayPressingShutterReached(void);
static void timerDelayDueToNumberOfPicture(sensorParameters_t sensorParameters);
static void handleManualCameraControl(void);

void CameraControl_init(void)
{
    cameraControl.cameraControlMode = CAMERA_MODE_MANUAL;
    cameraControl.triggerManualPicture = false;
}

void TIMER0_Handler(void)
{
    Timer0_clearInterrupt();
    isTimeForShooter = true;
}

void CameraControl_Control(void)
{
    switch(cameraControl.cameraControlMode)
    {
    case CAMERA_MODE_SENSOR:
      handleSensorCameraControl();
      break;
      
    case CAMERA_MODE_MANUAL:
      handleManualCameraControl();
      break;
      
    case CAMERA_MODE_DEACTIVATED:
      break;
      
    default:
      assert(false);
      break;
    }
}

bool CameraControl_isPictureTaken(void)
{
    if(pictureTaken)
    {
        pictureTaken = false;
        return true;
    }

    return false;
}

void CameraControl_setMode(CameraControlMode_t cameraControlMode)
{
    if (cameraControlMode != cameraControl.cameraControlMode)
    {
        cameraControl.cameraControlMode = cameraControlMode;
        cameraControl.triggerManualPicture = false;
    }
}

void CameraControl_triggerManualPicture(void)
{
    cameraControl.triggerManualPicture = true;
}

/* ----------------Intern functions--------------------------------------------- */

static void handleManualCameraControl(void)
{
   
    if(cameraControl.triggerManualPicture || isShootProcessRunning)
    {
        isShootProcessRunning = true;
        if (true == cameraControl.triggerManualPicture)
        {
            Timer0_setTimerCounter(8 * ONE_SECOND_TIMER_VALUE);
            cameraControl.triggerManualPicture = false;
        }
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

static void handleSensorCameraControl(void)
{
    sensorParameters_t sensorParameters = SensorControl_getParameters();
    
    switch(sensorParameters.shootMode)
    {
        case MODE_FOCUS_AND_SHOOT:
            processModeFocusAndShoot(sensorParameters);
            break;
        case MODE_CONTINUOUSFOCUS_AND_SHOOT:
            processModeContinuousFocus(sensorParameters);
            break;
        default:
            assert(false);
            break;
    }
}

static void processModeContinuousFocus(sensorParameters_t sensorParameters)
{
    GpioPortF_activatePin(PIN_AUTOFOCUS);

    if(sensorParameters.isInputTrigered || isShootProcessRunning)
    {
        isShootProcessRunning = true;

        if(sensorParameters.isFirstPictureToBeTaken)
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

static void processModeFocusAndShoot(sensorParameters_t sensorParameters)
{
    if(sensorParameters.isInputTrigered || isShootProcessRunning)
    {
        isShootProcessRunning = true;
        timerDelayDueToNumberOfPicture(sensorParameters);
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
        cameraControl.triggerManualPicture = false;
    }
    else
    {
        GpioPortF_activatePin(PIN_AUTOFOCUS | PIN_SHUTTER);
    }
}

static void timerDelayDueToNumberOfPicture(sensorParameters_t sensorParameters)
{
    if(sensorParameters.isFirstPictureToBeTaken && sensorParameters.isInputTrigered)
    {
        Timer0_setTimerCounter(8 * ONE_SECOND_TIMER_VALUE);
    }
    else if(sensorParameters.isInputTrigered)
    {
        Timer0_setTimerCounter(2*ONE_SECOND_TIMER_VALUE);
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


