#include "Service/Timebase/Timebase.h"

#include "HAL/CLOCK/Systick.h"

typedef struct 
{
    uint32_t time100ms;
}timebaseControl_t;

static timebaseControl_t timebaseControl;

void Timebase_init(void)
{
    timebaseControl.time100ms = 0U;
}

uint32_t Timebase_getValue(uint32_t offset100ms)
{
    return (timebaseControl.time100ms + offset100ms);
}

bool Timebase_isTimeout(uint32_t timeOut100ms)
{
    bool retVal = false;
    
    if (timebaseControl.time100ms >= timeOut100ms)
    {
        retVal = true;
    }
    
    return retVal;
}

void SysTick_Handler(void)
{
    timebaseControl.time100ms++;
}


/*----------------static functions------------------------*/


