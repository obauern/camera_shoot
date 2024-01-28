#include "Timer0.h"

#include "TM4C123GH6PM.h"
#include "tm4c_cmsis.h"

#define CLOCK_TIMER0_AVAILABLE  (1U << 0U)

#define DEACTIVATE_TIMER0       (~(1U << 0U))
#define ACTIVATE_TIMER0         (1U << 0U)

#define MODE_32_BITS        (0x00000000)

#define ONE_SHOOT_TIMER_MODE    (1U << 0U)
#define COUNTING_DOWN_MODE      (~(1U<<4U))

#define TIMER0_INTERRUPT_ENABLE  (1U << 0U)

#define ONE_SECOND_WITH_16_MHZ   (16000000U - 1U)

void Timer0_init(void)
{
    SYSCTL_RCGCTIMER_R |= CLOCK_TIMER0_AVAILABLE; /*clock gating for timer0*/
    
    TIMER0->CTL &= DEACTIVATE_TIMER0; 
    TIMER0->CFG = MODE_32_BITS;
    
    TIMER0->TAMR &= ~0x03;
    TIMER0->TAMR |= ONE_SHOOT_TIMER_MODE;
    TIMER0->TAMR &= COUNTING_DOWN_MODE;/*counting down*/
    
    TIMER0->TAILR = ONE_SECOND_WITH_16_MHZ; /*16Mhz, one second*/
    
    TIMER0->IMR |= TIMER0_INTERRUPT_ENABLE;
    
    NVIC_EnableIRQ(Timer0A_IRQn);
}

void Timer0_start(void)
{
    TIMER0->CTL |= ACTIVATE_TIMER0;
}
void Timer0_stop(void)
{
    TIMER0->CTL &= DEACTIVATE_TIMER0;
}
void Timer0_clearInterrupt(void)
{
    TIMER0->ICR |= 1U;
}
void Timer0_setTimerCounter(uint32_t counterValue)
{
    TIMER0->TAILR = counterValue;
}



