#ifndef __TIMER_3__
#define __TIMER_3__

#include <stdbool.h>

void Timer3_init(void);
void Timer3_clearCaptureInterrupt(void);
bool Timer3_isCaptureEventOccurred(void);

#endif  /*__TIMER_3__*/