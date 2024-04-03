/*
 * Page2.c
 *
 *  Created on: januar 2024
 *      Author: Alejandro Bauernfeind
 */
#include "Application/Visual/Pages/Page2.h"

#include <string.h>

#include "Application/Camera/output/CameraControl.h"
#include "Application/Visual/Tools/ili9341Button.h"
#include "Application/Visual/Tools/ili9341Display.h"
#include "Service/Timebase/Timebase.h"
#include "Service/TouchPanel/Panel/ili9341.h"
#include "Service/TouchPanel/Touch/ili9341Touch.h"
   
#define HIGHER_BUTTONS_HIGHT (60U)


#define  ONE_SECOND_TIMER (10U)

#define SECONDS_IN_MINUTE (60U)
#define SECONDS_IN_HOUR   (3600U)
   
   
   
static PagesTypes_t nextPage;
static uint8_t buttonTestId = 0;
static uint32_t testVariable = 20;
static uint8_t numberDisplay; 

typedef struct 
{
  uint8_t manualId;
  uint8_t sensorId;
  uint8_t enableId;
  uint8_t disableId;
  uint8_t incTimeId;
  uint8_t decTimeId;
}buttonTouchIds_t;
   
typedef struct
{
   uint8_t timerSetPoint;
   uint8_t timerIsPoint;
}displayIds_t;

typedef struct
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
}timer_t;
   
typedef struct 
{
    bool continuousTriggerEnabled;
    bool changeEnableDisableContinous;
    uint32_t setPointSeconds;
    uint32_t isPointSeconds;
    uint32_t timer;
    buttonTouchIds_t buttonTouchIds; 
    displayIds_t     displayIds;
}page2Control_t;

static page2Control_t page2Control;

static uint8_t initManualButton(void);
static uint8_t initSensorButton(void);
static uint8_t initEnableButton(void);
static uint8_t initDisableButton(void);
static uint8_t initIncTimeButton(void);
static uint8_t initDecTimeButton(void);
static void disableButtonFunction(void);
static void enableButtonFunction(void);
static void buttonTestFunction(void);
static void controlEnableDisable(void);
static uint8_t initTimerSetPointDisplay(void);
static uint8_t initTimerIsPointDisplay(void);
static void incTimeButtonFunction(void);
static void decTimeButtonFunction(void);
static void controlIsTimerDisplay(void);
static uint32_t convertHoursMinutesSecondsToSeconds(timer_t timer);
static void convertSecondsToHoursMinutesSeconds(uint32_t seconds, timer_t* timer);
   
void Page2_execute(void)
{
    TouchData_t touchData;
    touchData.x = 0;
    touchData.y = 0;
    if (false == ILI9341Touch_isPressed())
    {
        if(ILI9341Touch_getCoordinates(&touchData)==true)
        {
            (void)ili9341Button_Touch(&touchData);
        }
        //nextPage = PAGES_NUMBER_1;
    }
    else
    {
        (void)ili9341Button_Touch(&touchData);
        controlEnableDisable();
    }
    
    if (true == page2Control.continuousTriggerEnabled)
    {
        if (Timebase_isTimeout(page2Control.timer))
        {
            page2Control.timer = Timebase_getValue(page2Control.setPointSeconds*10);
            CameraControl_triggerManualPicture();
        }
    }
    else
    {
        page2Control.timer = 0U;
    }
    
    controlIsTimerDisplay();
}

void Page2_showPage(void)
{
    memset(&page2Control.buttonTouchIds,0x00,sizeof(buttonTouchIds_t));
    memset(&page2Control.displayIds,0x00,sizeof(displayIds_t));
    page2Control.continuousTriggerEnabled = false;
    page2Control.changeEnableDisableContinous = true;
    page2Control.timer = 0U;
    page2Control.setPointSeconds = 30U;
    page2Control.isPointSeconds = 0U;
  
    ili9341Button_DeleteAll();
    
    
    page2Control.buttonTouchIds.manualId = initManualButton();
    page2Control.buttonTouchIds.sensorId = initSensorButton();
    page2Control.buttonTouchIds.disableId = initDisableButton();
    page2Control.buttonTouchIds.enableId = initEnableButton();
    page2Control.buttonTouchIds.incTimeId = initIncTimeButton();
    page2Control.buttonTouchIds.decTimeId = initDecTimeButton();

    page2Control.displayIds.timerSetPoint = initTimerSetPointDisplay();
    page2Control.displayIds.timerIsPoint = initTimerIsPointDisplay();

    
    ILI9341_FillScreen(ILI9341_BLACK);
    
    ili9341Button_Draw(page2Control.buttonTouchIds.manualId);
    ili9341Button_Draw(page2Control.buttonTouchIds.sensorId);
    ili9341Button_Draw(page2Control.buttonTouchIds.disableId);
    ili9341Button_Draw(page2Control.buttonTouchIds.enableId);
    ili9341Button_Draw(page2Control.buttonTouchIds.incTimeId);
    ili9341Button_Draw(page2Control.buttonTouchIds.decTimeId);
    
    ili9341Display_Draw(page2Control.displayIds.timerSetPoint);
    ili9341Display_Draw(page2Control.displayIds.timerIsPoint);
    //ili9341Button_Draw(buttonTestId);
    //ili9341Button_DrawAll();

        
    nextPage = PAGES_NUMBER_2;
}

PagesTypes_t Page2_nextPage(void)
{
    return nextPage;
}

/*------INTERNAL VARIABLES------------*/

static uint8_t initManualButton(void)
{
    TM_ILI9341_Button_t manualButton;
    
    manualButton.x = ILI9341_TOUCH_SCALE_X/2;
    manualButton.y = 0;
    manualButton.width = ILI9341_TOUCH_SCALE_X/2-20;
    manualButton.height = HIGHER_BUTTONS_HIGHT;
    manualButton.background = ILI9341_RED;
    manualButton.pressedBackground = ILI9341_RED;
    manualButton.label = "MANUAL";
    manualButton.font = &Font_11x18;
    manualButton.color = ILI9341_BLACK;
    manualButton.callbackFunction = 0x00;
    
    return ili9341Button_Add(&manualButton);
}

static uint8_t initSensorButton(void)
{
    TM_ILI9341_Button_t sensorButton;
    
    sensorButton.x = 0;
    sensorButton.y = 0;
    sensorButton.width = ILI9341_TOUCH_SCALE_X/2-10;
    sensorButton.height = HIGHER_BUTTONS_HIGHT;
    sensorButton.background = ILI9341_YELLOW;
    sensorButton.pressedBackground = ILI9341_RED;
    sensorButton.label = "SENSOR";
    sensorButton.font = &Font_11x18;
    sensorButton.color = ILI9341_BLACK;
    sensorButton.callbackFunction = 0x00;
    
    return ili9341Button_Add(&sensorButton);
}

static uint8_t initDisableButton(void)
{
  TM_ILI9341_Button_t disableButton;
    
    disableButton.x = 0;
    disableButton.y = HIGHER_BUTTONS_HIGHT + 2;
    disableButton.width = ILI9341_TOUCH_SCALE_X/2-10;
    disableButton.height = 20;
    disableButton.background = ILI9341_YELLOW;
    disableButton.pressedBackground = ILI9341_RED;
    disableButton.label = "Disabled";
    disableButton.font = &Font_11x18;
    disableButton.color = ILI9341_BLACK;
    disableButton.callbackFunction = disableButtonFunction;

    return ili9341Button_Add(&disableButton);
}

static uint8_t initEnableButton(void)
{
    TM_ILI9341_Button_t enableButton;
    
    enableButton.x = ILI9341_TOUCH_SCALE_X/2;
    enableButton.y = HIGHER_BUTTONS_HIGHT + 2;
    enableButton.width = ILI9341_TOUCH_SCALE_X/2-10;
    enableButton.height = 20;
    enableButton.background = ILI9341_YELLOW;
    enableButton.pressedBackground = ILI9341_RED;
    enableButton.label = "Enabled";
    enableButton.font = &Font_11x18;
    enableButton.color = ILI9341_BLACK;
    enableButton.callbackFunction = enableButtonFunction;

    return ili9341Button_Add(&enableButton);
}

static uint8_t initIncTimeButton(void)
{
    TM_ILI9341_Button_t incTimeButton;
    
    incTimeButton.x = ILI9341_TOUCH_SCALE_X/2+5;
    incTimeButton.y = 90;
    incTimeButton.width = 25;
    incTimeButton.height = 30;
    incTimeButton.background = ILI9341_YELLOW;
    incTimeButton.pressedBackground = ILI9341_RED;
    incTimeButton.label = "->";
    incTimeButton.font = &Font_11x18;
    incTimeButton.color = ILI9341_BLACK;
    incTimeButton.callbackFunction = incTimeButtonFunction;

    return ili9341Button_Add(&incTimeButton);
}

static uint8_t initDecTimeButton(void)
{
    TM_ILI9341_Button_t decTimeButton;
    
    decTimeButton.x = 0;
    decTimeButton.y = 90;
    decTimeButton.width = 25;
    decTimeButton.height = 30;
    decTimeButton.background = ILI9341_YELLOW;
    decTimeButton.pressedBackground = ILI9341_RED;
    decTimeButton.label = "<-";
    decTimeButton.font = &Font_11x18;
    decTimeButton.color = ILI9341_BLACK;
    decTimeButton.callbackFunction = decTimeButtonFunction;

    return ili9341Button_Add(&decTimeButton);
}

static void incTimeButtonFunction(void)
{
    page2Control.setPointSeconds += 30;
    ili9341Display_Draw(page2Control.displayIds.timerSetPoint);
}

static void decTimeButtonFunction(void)
{
    if(page2Control.setPointSeconds >= 60)
    {
        page2Control.setPointSeconds -= 30;
    }
    ili9341Display_Draw(page2Control.displayIds.timerSetPoint);
}

static void disableButtonFunction(void)
{
    page2Control.continuousTriggerEnabled = false;
    page2Control.changeEnableDisableContinous = true;
}

static void enableButtonFunction(void)
{
    page2Control.continuousTriggerEnabled = true;
    page2Control.changeEnableDisableContinous = true;
}

static void buttonTestFunction(void)
{
    //TM_ILI9341_Button_DeleteAll();
    //nextPage = PAGES_NUMBER_1;
    testVariable += 50;
    (void)ili9341Display_Draw(numberDisplay);
    page2Control.continuousTriggerEnabled = true;
}

static uint8_t initTimerSetPointDisplay(void)
{
    ILI9341_Display_t timerSetPointDisplay;
    
    timerSetPointDisplay.x = 50;
    timerSetPointDisplay.y = 90;
    timerSetPointDisplay.width = 60;
    timerSetPointDisplay.height = 30;
    timerSetPointDisplay.background = ILI9341_YELLOW;
    timerSetPointDisplay.variable = &page2Control.setPointSeconds;
    timerSetPointDisplay.font = &Font_11x18;
    timerSetPointDisplay.color = ILI9341_BLACK;
    
    return ili9341Display_Add(&timerSetPointDisplay);
}
static uint8_t initTimerIsPointDisplay(void)
{
    ILI9341_Display_t timerIsPointDisplay;
    
    timerIsPointDisplay.x = 50;
    timerIsPointDisplay.y = 125;
    timerIsPointDisplay.width = 60;
    timerIsPointDisplay.height = 30;
    timerIsPointDisplay.background = ILI9341_YELLOW;
    timerIsPointDisplay.variable = &page2Control.isPointSeconds;
    timerIsPointDisplay.font = &Font_11x18;
    timerIsPointDisplay.color = ILI9341_BLACK;
    
    return ili9341Display_Add(&timerIsPointDisplay);
}

static void controlEnableDisable(void)
{
    if (true == page2Control.changeEnableDisableContinous)
    {
        page2Control.changeEnableDisableContinous = false;
        if (true == page2Control.continuousTriggerEnabled)
        {
            ili9341Button_DrawButtonWithBackgroundPressed(page2Control.buttonTouchIds.enableId);
            ili9341Button_Draw(page2Control.buttonTouchIds.disableId);
        }
        else
        {
            ili9341Button_DrawButtonWithBackgroundPressed(page2Control.buttonTouchIds.disableId);
            ili9341Button_Draw(page2Control.buttonTouchIds.enableId);
        }
    }
}

static void controlIsTimerDisplay(void)
{
    uint32_t differenceTimer = Timebase_getDifference(page2Control.timer);
    
    differenceTimer /= 10;
    
    if (differenceTimer != page2Control.isPointSeconds)
    {
        page2Control.isPointSeconds = differenceTimer;
        ili9341Display_Draw(page2Control.displayIds.timerIsPoint);
    }
}

static uint32_t convertHoursMinutesSecondsToSeconds(timer_t timer)
{
    uint32_t seconds = 0U;
    
    seconds = timer.seconds;
    seconds += SECONDS_IN_MINUTE*timer.minutes;
    seconds += SECONDS_IN_HOUR*timer.hours;
    
    return seconds;
}

static void convertSecondsToHoursMinutesSeconds(uint32_t seconds, timer_t* timer)
{
    timer->hours = seconds / SECONDS_IN_HOUR;
    while (seconds >= SECONDS_IN_HOUR)
    {
        seconds -= SECONDS_IN_HOUR;
    }
    timer->minutes = seconds / SECONDS_IN_MINUTE;
    while (seconds >= SECONDS_IN_MINUTE)
    {
        seconds -= SECONDS_IN_MINUTE;
    }
    timer->seconds = seconds;
}
