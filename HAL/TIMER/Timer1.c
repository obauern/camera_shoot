#include "Timer1.h"

#include "TM4C123GH6PM.h"
#include "tm4c_cmsis.h"

#define CLOCK_TIMER1_AVAILABLE  (1U << 1U)

#define DEACTIVATE_TIMER1       (0x00000000)
#define ACTIVATE_TIMER1         (1U << 0U)

#define MODE_32_BITS        (0x00000000)

#define PERIODIC_TIMER_MODE    (1U << 1U)
#define COUNTING_DOWN_MODE      (~(1U<<4U))

#define TIMER1_INTERRUPT_ENABLE  (1U << 0U)

#define TIME_100MS_WITH_16_MHZ       (1600000U - 1U)

static void initTimerTriggerHCSr04(void);

void Timer1_init(void)
{
    initTimerTriggerHCSr04();
}

void Timer1_clearInterrupt(void)
{
    TIMER1->ICR |= 1;
}       

/*---------------INTERNAL FUNCTIONS-------------------*/
static void initTimerTriggerHCSr04(void)
{
    SYSCTL_RCGCTIMER_R |= CLOCK_TIMER1_AVAILABLE; /*clock gating for timer1*/
    
    TIMER1->CTL = DEACTIVATE_TIMER1;
    TIMER1->CFG = MODE_32_BITS;
    
    TIMER1->TAMR &= ~0x03;
    TIMER1->TAMR |= PERIODIC_TIMER_MODE;
    TIMER1->TAMR &= COUNTING_DOWN_MODE;
    
    TIMER1->TAILR = TIME_100MS_WITH_16_MHZ;
    
    TIMER1->IMR |= TIMER1_INTERRUPT_ENABLE;
    
    NVIC_EnableIRQ(Timer1A_IRQn);

    TIMER1->CTL |= ACTIVATE_TIMER1;
}



