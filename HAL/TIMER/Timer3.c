#include "Timer3.h"

#include "TM4C123GH6PM.h"
#include "tm4c_cmsis.h"

#define CLOCK_TIMER3_AVAILABLE  (1U << 3U)

#define DEACTIVATE_TIMER3       (~(1U << 0U))
#define ACTIVATE_TIMER3         (1U << 0U)

#define MODE_16_BIT             (1U << 2U)

#define CAPTURE_TIMER_MODE      (7U << 0U)
#define COUNTING_UP_MODE        (1U << 4U)

#define MAXIMUM_LIMIT_TIME      (0xFFFFFFFF)

#define CAPTURE_BOTH_EDGES      (3U << 2U)

#define NO_PREESCALER            (0x00000000)

#define ACTIVATE_INTERRUPT_CAPTURE (1U << 2U)

#define CAPTURE_EVENT_INTERRUPT    (1U << 2U)

void Timer3_init(void)
{
    SYSCTL_RCGCTIMER_R |= CLOCK_TIMER3_AVAILABLE; /*clock gating for timer3*/
    
    TIMER3->CTL &= DEACTIVATE_TIMER3; 
    
    TIMER3->CFG = MODE_16_BIT;

    TIMER3->TAMR = CAPTURE_TIMER_MODE | COUNTING_UP_MODE;

    TIMER3->TAILR = MAXIMUM_LIMIT_TIME; 
    
    TIMER3->CTL |= CAPTURE_BOTH_EDGES;
       
    TIMER3->TAPR = NO_PREESCALER;
    
    TIMER3->IMR |= ACTIVATE_INTERRUPT_CAPTURE;
       
    NVIC_EnableIRQ(Timer3A_IRQn);
    
    TIMER3->CTL |= ACTIVATE_TIMER3;
}

void Timer3_clearCaptureInterrupt(void)
{
    TIMER3->ICR |= CAPTURE_EVENT_INTERRUPT;
}

bool Timer3_isCaptureEventOccurred(void)
{
    return (TIMER3->RIS & CAPTURE_EVENT_INTERRUPT);
}

uint32_t Timer3_valueCountRegister(void)
{
    return TIMER3->TAR;
}



