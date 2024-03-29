#ifndef __TIMER_3__
#define __TIMER_3__

#include <stdbool.h>
#include <stdint.h>

void Timer3_init(void);
void Timer3_clearCaptureInterrupt(void);
bool Timer3_isCaptureEventOccurred(void);
uint32_t Timer3_valueCountRegister(void);

#endif  /*__TIMER_3__*/