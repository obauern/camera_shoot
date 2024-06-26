#include "init.h"

#include <stdint.h>
#include <stdbool.h>

#include "Application/Camera/input/SensorControl.h"
#include "TM4C123GH6PM.h"
#include "tm4c_cmsis.h"
#include "HAL/CLOCK/clockRegister.h"
#include "HAL/SPI/HalSpi.h"
#include "HAL/GPIO/GpioPortA.h"
#include "HAL/GPIO/GpioPortB.h"
#include "HAL/GPIO/GpioPortF.h"
#include "HAL/CLOCK/Systick.h"
#include "HAL/TIMER/Timer0.h"
#include "HAL/TIMER/Timer1.h"
#include "HAL/TIMER/Timer2.h"
#include "HAL/TIMER/Timer3.h"



void initialise_registers(void)
{
    ClockRegister_init();
    HalSpi_init();
    GpioPortA_init();
    GpioPortB_init();
    GpioPortF_init();
    
    Timer0_init();
    
    /*Timers for HCSr04*/
    Timer1_init();  /*Trigger*/    
    Timer2_init(); 
    Timer3_init(); /*Echo measurements*/    
    
    SysTick_init();
    
    SensorControl_Init();
}

/*----------------static functions------------------------*/


