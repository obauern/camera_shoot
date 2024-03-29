#include "Timer2.h"

#include "TM4C123GH6PM.h"
#include "tm4c_cmsis.h"

#define CLOCK_TIMER2_AVAILABLE  (1U << 2U)

#define DEACTIVATE_TIMER2       (0x00000000)
#define ACTIVATE_TIMER2         (1U << 0U)

#define MODE_16_BIT        (1U << 2U)

#define PERIODIC_TIMER_MODE    (1U << 1U)
#define COUNTING_DOWN_MODE      (~(1U<<4U))

#define TIMER0_INTERRUPT_ENABLE  (1U << 0U)

#define ONE_US_WITH_16_MHZ       (16U - 1U)

#define CLEAR_INTERRUPT_TIMER2   (1U << 0U)

#define IS_TIMER2_TIMED_OUT     (1U << 0U)

void Timer2_init(void)
{
    SYSCTL_RCGCTIMER_R |= CLOCK_TIMER2_AVAILABLE; /*clock gating for timer2*/
    
    TIMER2->CTL &= DEACTIVATE_TIMER2; 
    
    TIMER2->CFG |= MODE_16_BIT;
    
    TIMER2->TAMR &= ~0x03;
    TIMER2->TAMR |= PERIODIC_TIMER_MODE;
    TIMER2->TAMR &= COUNTING_DOWN_MODE;/*counting down*/
    
    TIMER2->TAILR = ONE_US_WITH_16_MHZ; /*16Mhz, one microsecond*/
    
    TIMER2->ICR |= CLEAR_INTERRUPT_TIMER2;
    
    TIMER2->CTL |= ACTIVATE_TIMER2;
}

void Timer2_clearInterrupt(void)
{
    TIMER2->ICR |= 1;
}

bool Timer2_isTimedOut(void)
{
    return ((TIMER2->RIS & IS_TIMER2_TIMED_OUT) == 0);
}



