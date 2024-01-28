#ifndef __TIMER_0__
#define __TIMER_0__

#include <stdint.h>

void Timer0_init(void);
void Timer0_start(void);
void Timer0_stop(void);
void Timer0_clearInterrupt(void);
void Timer0_setTimerCounter(uint32_t counterValue);

#endif  /*__TIMER_0__*/