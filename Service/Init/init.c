#include "init.h"

#include "HAL/GPIO/GpioPortB.h"
#include "HAL/GPIO/GpioPortF.h"
#include "HAL/TIMER/Timer0.h"
#include "HAL/TIMER/Timer1.h"
#include "HAL/TIMER/Timer2.h"
#include "HAL/TIMER/Timer3.h"



void initialise_values(void)
{
    GpioPortB_init();
    GpioPortF_init();
    
    Timer0_init();
    
    /*Timers for HCSr04*/
    Timer1_init();  /*Trigger*/    
    Timer2_init(); 
    Timer3_init(); /*Echo measurements*/     
    
    initSensorParameters();
}

/*----------------static functions------------------------*/


