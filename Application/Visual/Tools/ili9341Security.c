#include "Application/Visual/Tools/ili9341Security.h"

#include <stdint.h>
#include <stdbool.h>

#include "Application/Visual/Pages/HandlePages.h"
#include "Service/Timebase/Timebase.h"
#include "Service/TouchPanel/Panel/ili9341.h"


#define ONE_SECOND (10U)

#define DISPLAY_ON (1U << 2U)
#define DISPLAY_NORMAL_MODE (1U << 3U)
#define SLEEP_OUT_MODE (1U << 4U)
#define PARTIAL_MODE_OFF (1U << 5U)
#define IDLE_MODE_OFF (1U << 6U)
#define BOOSTER_ON_OK (1U << 7U)

typedef struct
{
   uint32_t timer;
   uint8_t iliPower;
}securityControl_t;

static bool controlPowerMode(uint8_t powerMode);

static securityControl_t securityControl;

void ili9341Security_init(void)
{
    securityControl.timer = Timebase_getValue(ONE_SECOND);
    securityControl.iliPower = 0U;
}

void ili9341Security_controlPowerMode(void)
{
  
    if (true == Timebase_isTimeout(securityControl.timer))
    {
        securityControl.timer = Timebase_getValue(ONE_SECOND);
        securityControl.iliPower = ILI9341_ReadPowerMode();
        if (false == controlPowerMode(securityControl.iliPower))
        {
            ILI9341_Init();
            HandlePages_reset();
        }
    }
}

static bool controlPowerMode(uint8_t powerMode)
{
    bool retVal = false;
    
    if (0x00 == (powerMode & DISPLAY_ON))
    {
        retVal = false;
    }
    else if (0x00 == (powerMode & DISPLAY_NORMAL_MODE))
    {
        retVal = false;
    }
    else if (0x00 == (powerMode & SLEEP_OUT_MODE))
    {
        retVal = false;
    }
    else if (0x00 != (powerMode & PARTIAL_MODE_OFF))
    {
        retVal = false;
    }
    else if (0x00 != (powerMode & IDLE_MODE_OFF))
    {
        retVal = false;
    }
    else if (0x00 == (powerMode & BOOSTER_ON_OK))
    {
        retVal = false;
    }
    else
    {
        retVal = true;
    }
    
    return retVal;
}