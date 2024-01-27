#include "Timer0.h"

#include "TM4C123GH6PM.h"
#include "tm4c_cmsis.h"

#define CLOCK_TIMER0_AVAILABLE  (1U << 0U)

#define DEACTIVATE_TIMER0       (~(1U << 0U))

#define MODE_32_BITS        (0x00000000)

#define ONE_SHOOT_TIMER_MODE    (1U << 0U)
#define COUNTING_DOWN_MODE      (~(1U<<4U))

#define TIMER0_INTERRUPT_ENABLE  (1U << 0U)

#define ONE_SECOND_WITH_16_MHZ   (16000000U - 1U)

void Timer0_init(void)
{
    SYSCTL_RCGCTIMER_R |= CLOCK_TIMER0_AVAILABLE; /*clock gating for timer0*/
    
    TIMER0_CTL_R &= DEACTIVATE_TIMER0; 
    TIMER0_CFG_R = MODE_32_BITS;
    
    TIMER0_TAMR_R &= ~0x03;
    TIMER0_TAMR_R |= ONE_SHOOT_TIMER_MODE;
    TIMER0_TAMR_R &= COUNTING_DOWN_MODE;/*counting down*/
    
    TIMER0_TAILR_R = ONE_SECOND_WITH_16_MHZ; /*16Mhz, one second*/
    
    TIMER0->IMR |= TIMER0_INTERRUPT_ENABLE;
    
    NVIC_EnableIRQ(Timer0A_IRQn);
}



