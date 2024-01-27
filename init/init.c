#include "init.h"

#include "HAL/GPIO/GpioPortB.h"
#include "HAL/GPIO/GpioPortF.h"
#include "HAL/TIMER/Timer0.h"
#include "HAL/TIMER/Timer1.h"
#include "HAL/TIMER/Timer2.h"
#include "HAL/TIMER/Timer3.h"

#define TIMER_0 (1U << 0)

inline static void initialiseTimer0Interrupt(void);
inline static void initialiseGpio(void);

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


inline static void initialiseTimer0Interrupt(void)
{
    SYSCTL_RCGCTIMER_R |= TIMER_0; /*clock gating for timer0*/
    TIMER0_CTL_R &= ~(1 << 0); /*deactivate timer0*/
    TIMER0_CFG_R = 0x00000000;
    TIMER0_TAMR_R |= 0x1;
    TIMER0_TAMR_R &= ~(1<<4);/*counting down*/
    TIMER0_TAILR_R = 0x00F42400; /*16Mhz, one second*/
    
    TIMER0->IMR |= (1<<0);
    NVIC_EnableIRQ(Timer0A_IRQn);
}

