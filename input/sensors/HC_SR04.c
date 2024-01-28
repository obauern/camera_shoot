#include "HC_SR04.h"

#include "HAL/GPIO/GpioPortB.h"
#include "HAL/TIMER/Timer1.h"
#include "HAL/TIMER/Timer2.h"
#include "HAL/TIMER/Timer3.h"

#define PB2 (1<<2U)
#define PB3 (1<<3U)
#define PF0 (1<<0U)
#define SW1 (1<<4U)

#define SOUND_V_CM_US 0.0343
#define MINIMUM_DISTANCE_CM 20.0
#define NUMBERS_OF_PICTURES 3

#define PB2_GPIO_PIN   (1U << 2U)

static bool isObjectDetected = false;
static bool flagForNextPicture = true;
static bool pictureToBeTaken = false;
static uint8_t numbersOfPictures = 0;
static bool internIsPictureTaked = false;
static bool isDataAvailable = false;
static bool isTimeOfNewMeasurement = false;
static float distanceEchoCm = 0;
static uint32_t risingEdgeTime = 0;
static uint32_t fallingEdgeTime = 0;

static void delayHcSr04(uint32_t microseconds);
static void HcSr04TriggerControl(void);
static void HcSr04DataProcessingControl(void);
static float calculateDistanceMeassured(void);
static bool conditionsForPictureAreMet(void);
static void calculateLimitsOfDistance(void);
static void calculateIfPicturesCanBeTaken(void);


void delayHcSr04(uint32_t microseconds)
{
  // Generar el retardo
    for (uint32_t i = 0; i < microseconds; i++) 
    {
        while (Timer2_isTimedOut());    // Esperar a que se active la bandera de interrupción de Timer2A
        Timer2_clearInterrupt();                   // Limpiar la bandera de interrupción de Timer2A
    }
}

void TIMER1A_Handler(void)
{
    Timer1_clearInterrupt();
    isTimeOfNewMeasurement = true;
}

void TIMER3A_Handler(void) /*Interruption for timer to measure the echo pulse*/
{
    if(Timer3_isCaptureEventOccurred()) /*If it is capture mode in timer3a*/
    {
        Timer3_clearCaptureInterrupt();
        if(GpioPortB_readPin(PB2_GPIO_PIN)) /*rising register*/
        {
            risingEdgeTime = Timer3_valueCountRegister();
        }
        else
        {
            fallingEdgeTime = Timer3_valueCountRegister();
            isDataAvailable = true;
        }
    }
}

/**/
void HcSr04Control(bool isPictureTaked)
{
    if(isPictureTaked)
    {
        internIsPictureTaked = true;
    }
    HcSr04TriggerControl();
    HcSr04DataProcessingControl();
}

bool isAnObjectDetected(void)
{
    if(pictureToBeTaken)
    {
        pictureToBeTaken = false;
        return true;
    }
    return false;
}

bool isFirstPictureToBeTaken(void)
{
    return (numbersOfPictures <= 1); /*1 because when the first picture comes numberOfPictures++*/
}

/*-------------INTERNAL FUNCTIONS----------------*/

static void HcSr04TriggerControl(void)
{
    if(isTimeOfNewMeasurement)
    {
        isTimeOfNewMeasurement = false;
        GPIOB_HS->DATA |= PB3;/*PB3 trigger activated*/
        delayHcSr04(10);      /*delay of 10 microseconds*/
        GPIOB_HS->DATA &= ~PB3;/*PB3 trigger deactivated*/
    }
}

static void HcSr04DataProcessingControl(void)
{
    if(isDataAvailable)
    {
        isDataAvailable = false;
        distanceEchoCm = calculateDistanceMeassured();
        calculateLimitsOfDistance();
        calculateIfPicturesCanBeTaken();
    }
}

static void calculateIfPicturesCanBeTaken(void)
{
    if(conditionsForPictureAreMet())
    {
        pictureToBeTaken = true;
        flagForNextPicture = false;
        numbersOfPictures++;
    }
    
    if(internIsPictureTaked)
    {
        flagForNextPicture = true;
        internIsPictureTaked = false;
    }
    
    if(!isObjectDetected)
    {
        numbersOfPictures = 0;
        flagForNextPicture = true;
    }
}

static bool conditionsForPictureAreMet(void)
{
    return (isObjectDetected && numbersOfPictures<NUMBERS_OF_PICTURES && flagForNextPicture);
}

static void calculateLimitsOfDistance(void)
{
    if(distanceEchoCm < MINIMUM_DISTANCE_CM)
    {
        isObjectDetected = true;
    }
    else
    {
        isObjectDetected = false;
    }
}

static float calculateDistanceMeassured(void)
{
    static uint32_t totallyTime;
    static uint32_t echoTimeMicroSeconds = 0;


    totallyTime = fallingEdgeTime - risingEdgeTime;
    echoTimeMicroSeconds = totallyTime/16;
    return (echoTimeMicroSeconds*SOUND_V_CM_US/2);
}