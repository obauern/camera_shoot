#include "HC_SR04.h"

#define PB2 (1<<2U)
#define PB3 (1<<3U)
#define PF0 (1<<0U)
#define SW1 (1<<4U)

#define SOUND_V_CM_US 0.0343
#define MINIMUM_DISTANCE_CM 20.0
#define NUMBERS_OF_PICTURES 3
#define TIMER1_100MS    (1600000U - 1U)
#define TIMER2_1US    (16U - 1U)

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

static void initTimer3ForEchoMeasurement(void);
static void initTimer2ForTrigger(void);
static void activateGpioBHs(void);
static void initTimer1ForTimeOfTrigger(void);
static void initGpioPb2ForT3CCP0(void);
static void initGpioPb3AsTrigger(void);
static void delayHcSr04(uint32_t microseconds);
static void HcSr04TriggerControl(void);
static void HcSr04DataProcessingControl(void);
static float calculateDistanceMeassured(void);
static bool conditionsForPictureAreMet(void);
static void calculateLimitsOfDistance(void);
static void calculateIfPicturesCanBeTaken(void);

/*Initialisation of timers for the module*/
void initTimersForHCSr04(void)
{
    initTimer1ForTimeOfTrigger();
    initTimer2ForTrigger();
    initTimer3ForEchoMeasurement();
}

static void initTimer1ForTimeOfTrigger(void)
{
    // Configurar Timer2A
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;      // Habilitar el reloj para Timer1
    TIMER1->CTL = 0x00000000;       // Deshabilitar Timer1A durante la configuración
    TIMER1->CFG = 0x00000000;       // Timer1A en modo de temporizador de 32 bits
    TIMER1->TAMR = 0x00000002;      // Timer1A en modo periódico and the timer counts down
    TIMER1->TAILR = TIMER1_100MS;   // Valor de recarga para generar un retardo de 100 msegundos (16 MHz)
    
    TIMER1->IMR |= (1<<0);          /*Unmasks the interrupt*/
    NVIC_EnableIRQ(Timer1A_IRQn);
    TIMER1->CTL |= 0x00000001;      // Habilitar Timer1A
}

static void initTimer2ForTrigger(void)
{
    // Configurar Timer2A
    SYSCTL_RCGCTIMER_R |= 0x04;      // Habilitar el reloj para Timer2
    TIMER2->CTL = 0x00000000;       // Deshabilitar Timer2A durante la configuración
    TIMER2->CFG = 0x00000004;       // Timer2A en modo de temporizador de 16 bits
    TIMER2->TAMR = 0x00000002;      // Timer2A en modo periódico and the timer counts down
    TIMER2->TAILR = TIMER2_1US;         // Valor de recarga para generar un retardo de 1 micros (16 ciclos a 16 MHz)
    TIMER2->ICR = 0x00000001;       // Limpiar la bandera de interrupción de Timer2A
    TIMER2->CTL |= 0x00000001;      // Habilitar Timer2A
}

void delayHcSr04(uint32_t microseconds)
{
  // Generar el retardo
    for (uint32_t i = 0; i < microseconds; i++) {
        while ((TIMER2->RIS & 0x00000001) == 0);    // Esperar a que se active la bandera de interrupción de Timer2A
        TIMER2->ICR = 0x00000001;                   // Limpiar la bandera de interrupción de Timer2A
    }
}

static void initTimer3ForEchoMeasurement(void)
{
    SYSCTL_RCGCTIMER_R |= (1U<<3U); /*clock gating for timer3*/
    TIMER3->CTL &= ~(1u << 0); /*deactivate timer3*/
    TIMER3->CFG = 0x00000004; /* Timer3A in 16-bit timer mode */
    TIMER3->TAMR = 0x17; /* Timer3A enable up-count, capture mod, input edge time mode*/
    TIMER3->TAILR = 0xFFFFFFFF; /*Limit maximum for timer3*/
    TIMER3->CTL |= (1<<3u)|(1<<2u); /*capture in high and falling edge*/   
    TIMER3->TAPR = 0x00000000; /*no preescaler*/
    TIMER3->IMR |= (1<<2u);/*interrupt for capture modus in timer 3 activated*/
    NVIC_EnableIRQ(Timer3A_IRQn);
    
    
    TIMER3->CTL |= (1u<<0u); /*enables timer 3*/
}

void TIMER1A_Handler(void)
{
    TIMER1->ICR |= 1;
    isTimeOfNewMeasurement = true;
}

void TIMER3A_Handler(void) /*Interruption for timer to measure the echo pulse*/
{
    if((TIMER3->RIS & (1u<<2u))) /*If it is capture mode in timer3a*/
    {
        TIMER3->ICR |= (1u<<2u);/*Clears timer a capture for timer 3*/
        if((GPIOB_HS->DATA & PB2) != 0) /*rising register*/
        {
            risingEdgeTime = TIMER3->TAR;
        }
        else
        {
            fallingEdgeTime = TIMER3->TAR;
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

void initGpioForHCSr04(void)
{
    activateGpioBHs();
    initGpioPb2ForT3CCP0();
    initGpioPb3AsTrigger();
}

static void activateGpioBHs(void)
{
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;   /*gpio clock gate for port F*/
    SYSCTL_GPIOHBCTL_R |= SYSCTL_GPIOHBCTL_PORTB; /*activates the ahb*/
}

static void initGpioPb2ForT3CCP0(void)
{
    GPIOB_HS->DIR &= ~PB2; /*PB2 an input pin*/
    GPIOB_HS->DEN |= PB2;  /*Pb2 digital pin*/
    
    GPIOB_HS->AFSEL |= PB2; /*Special function in PB2*/
    GPIOB_HS->PCTL &= ~0x00000F00;
    GPIOB_HS->PCTL |= 0x00000700; /*T3CCP0*/
}

static void initGpioPb3AsTrigger(void)
{
    GPIOB_HS->DIR |= 0x08;       // Configurar el PB3 como salida
    GPIOB_HS->DEN |= 0x08;       // Habilitar la función digital en PB3
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