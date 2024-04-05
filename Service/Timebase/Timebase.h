#ifndef __INIT_H__
#define __INIT_H__

#include <stdint.h>
#include <stdbool.h>

void Timebase_init(void);
uint32_t Timebase_getValue(uint32_t offset100ms);
uint32_t Timebase_getDifference(uint32_t timeOut100ms);
bool Timebase_isTimeout(uint32_t timeOut100ms);

#endif  /*__INIT_H__*/