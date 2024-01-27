#ifndef __TIMER_2__
#define __TIMER_2__

#include <stdbool.h>

void Timer2_init(void);
void Timer2_clearInterrupt(void);
bool Timer2_isTimedOut(void);

#endif  /*__TIMER_2__*/