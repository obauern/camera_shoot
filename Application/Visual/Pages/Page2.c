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

typedef struct 
{
  uint8_t manualId;
  uint8_t sensorId;
  uint8_t enableId;
  uint8_t disableId;
  uint8_t incTimeId;
  uint8_t decTimeId;
  uint8_t incHourId;
  uint8_t decHourId;
  uint8_t incMinuteId;
  uint8_t decMinuteId;
}buttonTouchIds_t;
   
typedef struct
{
   uint8_t timerSetPoint;
   uint8_t timerIsPoint;
   uint8_t timerHourSetPoint;
   uint8_t timerMinuteSetPoint;
   uint8_t timerHourIsPoint;
   uint8_t timerMinuteIsPoint;
   uint8_t timerSecondIsPoint;
}displayIds_t;

typedef struct
{
    uint32_t seconds;
    uint32_t minutes;
    uint32_t hours;
}timer_t;
   
typedef struct 
{
    bool continuousTriggerEnabled;
    bool changeEnableDisableContinous;
    timer_t setPointTimer;
    timer_t isPointTimer;
    uint32_t setPointSeconds;
    uint32_t isPointSeconds;
    uint32_t timer;
    buttonTouchIds_t buttonTouchIds; 
    displayIds_t     displayIds;
}page2Control_t;

static page2Control_t page2Control;

static void showPage(void);
static uint8_t initManualButton(void);
static uint8_t initSensorButton(void);
static uint8_t initEnableButton(void);
static uint8_t initDisableButton(void);
static uint8_t initIncHourIdButton(void);
static uint8_t initDecHourIdButton(void);
static uint8_t initIncMinuteIdButton(void);
static uint8_t initDecMinuteIdButton(void);
static uint8_t initHourSetPointDisplay(void);
static uint8_t initMinuteSetPointDisplay(void);
static uint8_t initHourIsPointDisplay(void);
static uint8_t initMinuteIsPointDisplay(void);
static uint8_t initSecondIsPointDisplay(void);
static void disableButtonFunction(void);
static void enableButtonFunction(void);
static void controlEnableDisable(void);
static void incHourButtonFunction(void);
static void decHourButtonFunction(void);
static void incMinuteButtonFunction(void);
static void decMinuteButtonFunction(void);
static void controlShootCamera(void);
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
    
    controlShootCamera();
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
    page2Control.setPointTimer.minutes = 1U;
    
    showPage();
       
    nextPage = PAGES_NUMBER_2;
}

void Page2_reShow(void)
{
    showPage();
}

PagesTypes_t Page2_nextPage(void)
{
    return nextPage;
}

/*------INTERNAL VARIABLES------------*/

static void showPage(void)
{
    
    ili9341Button_DeleteAll();
    ili9341Display_DeleteAll();
    
    
    page2Control.buttonTouchIds.manualId = initManualButton();
    page2Control.buttonTouchIds.sensorId = initSensorButton();
    page2Control.buttonTouchIds.disableId = initDisableButton();
    page2Control.buttonTouchIds.enableId = initEnableButton();
    page2Control.buttonTouchIds.incHourId = initIncHourIdButton();
    page2Control.buttonTouchIds.decHourId = initDecHourIdButton();
    page2Control.buttonTouchIds.incMinuteId = initIncMinuteIdButton();
    page2Control.buttonTouchIds.decMinuteId = initDecMinuteIdButton();

    page2Control.displayIds.timerHourSetPoint = initHourSetPointDisplay();
    page2Control.displayIds.timerMinuteSetPoint = initMinuteSetPointDisplay();
    page2Control.displayIds.timerHourIsPoint = initHourIsPointDisplay();
    page2Control.displayIds.timerMinuteIsPoint = initMinuteIsPointDisplay();
    page2Control.displayIds.timerSecondIsPoint = initSecondIsPointDisplay();

    
    ILI9341_FillScreen(ILI9341_BLACK);
    
    ili9341Button_Draw(page2Control.buttonTouchIds.manualId);
    ili9341Button_Draw(page2Control.buttonTouchIds.sensorId);
    ili9341Button_Draw(page2Control.buttonTouchIds.disableId);
    ili9341Button_Draw(page2Control.buttonTouchIds.enableId);
    ili9341Button_Draw(page2Control.buttonTouchIds.incHourId);
    ili9341Button_Draw(page2Control.buttonTouchIds.incMinuteId);
    ili9341Button_Draw(page2Control.buttonTouchIds.decHourId);
    ili9341Button_Draw(page2Control.buttonTouchIds.decMinuteId);   
    
    ili9341Display_Draw(page2Control.displayIds.timerHourSetPoint);
    ili9341Display_Draw(page2Control.displayIds.timerMinuteSetPoint);
    ili9341Display_Draw(page2Control.displayIds.timerHourIsPoint);
    ili9341Display_Draw(page2Control.displayIds.timerMinuteIsPoint);
    ili9341Display_Draw(page2Control.displayIds.timerSecondIsPoint);
}

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

static uint8_t initIncHourIdButton(void)
{
    TM_ILI9341_Button_t incHourButton;
    
    incHourButton.x = 0;
    incHourButton.y = 90;
    incHourButton.width = 25;
    incHourButton.height = 30;
    incHourButton.background = ILI9341_YELLOW;
    incHourButton.pressedBackground = ILI9341_RED;
    incHourButton.label = "+";
    incHourButton.font = &Font_11x18;
    incHourButton.color = ILI9341_BLACK;
    incHourButton.callbackFunction = incHourButtonFunction;

    return ili9341Button_Add(&incHourButton);
}
static uint8_t initDecHourIdButton(void)
{
    TM_ILI9341_Button_t decHourButton;
    
    decHourButton.x = 0;
    decHourButton.y = 150;
    decHourButton.width = 25;
    decHourButton.height = 30;
    decHourButton.background = ILI9341_YELLOW;
    decHourButton.pressedBackground = ILI9341_RED;
    decHourButton.label = "-";
    decHourButton.font = &Font_11x18;
    decHourButton.color = ILI9341_BLACK;
    decHourButton.callbackFunction = decHourButtonFunction;

    return ili9341Button_Add(&decHourButton);
}
static uint8_t initIncMinuteIdButton(void)
{
    TM_ILI9341_Button_t incMinuteButton;
    
    incMinuteButton.x = 30;
    incMinuteButton.y = 90;
    incMinuteButton.width = 25;
    incMinuteButton.height = 30;
    incMinuteButton.background = ILI9341_YELLOW;
    incMinuteButton.pressedBackground = ILI9341_RED;
    incMinuteButton.label = "+";
    incMinuteButton.font = &Font_11x18;
    incMinuteButton.color = ILI9341_BLACK;
    incMinuteButton.callbackFunction = incMinuteButtonFunction;

    return ili9341Button_Add(&incMinuteButton);
}
static uint8_t initDecMinuteIdButton(void)
{
    TM_ILI9341_Button_t decMinuteButton;
    
    decMinuteButton.x = 30;
    decMinuteButton.y = 150;
    decMinuteButton.width = 25;
    decMinuteButton.height = 30;
    decMinuteButton.background = ILI9341_YELLOW;
    decMinuteButton.pressedBackground = ILI9341_RED;
    decMinuteButton.label = "-";
    decMinuteButton.font = &Font_11x18;
    decMinuteButton.color = ILI9341_BLACK;
    decMinuteButton.callbackFunction = decMinuteButtonFunction;

    return ili9341Button_Add(&decMinuteButton);
}

static void incHourButtonFunction(void)
{
    if (page2Control.setPointTimer.hours >= 24U)
    {
        page2Control.setPointTimer.hours = 0U;
        if (page2Control.setPointTimer.minutes == 0U)
        {
            page2Control.setPointTimer.minutes = 1U;
            ili9341Display_Draw(page2Control.displayIds.timerMinuteSetPoint);
        }
    }
    else
    {
        page2Control.setPointTimer.hours++;
    }
    ili9341Display_Draw(page2Control.displayIds.timerHourSetPoint);
}

static void decHourButtonFunction(void)
{
    if (page2Control.setPointTimer.hours == 0U)
    {
        page2Control.setPointTimer.hours = 24U;
    }
    else
    {
        page2Control.setPointTimer.hours--;
        if ((page2Control.setPointTimer.minutes == 0U) && (page2Control.setPointTimer.hours == 0U))
        {
            page2Control.setPointTimer.minutes = 1U;
            ili9341Display_Draw(page2Control.displayIds.timerMinuteSetPoint);
        }
    }
  ili9341Display_Draw(page2Control.displayIds.timerHourSetPoint);
}

static void incMinuteButtonFunction(void)
{
    if (page2Control.setPointTimer.minutes >= 59U)
    {
        if (page2Control.setPointTimer.hours == 0U)
        {
            page2Control.setPointTimer.minutes = 1U;
        }
        else
        {
          page2Control.setPointTimer.minutes = 0U;
        }
    }
    else
    {
        page2Control.setPointTimer.minutes++;
    }
  ili9341Display_Draw(page2Control.displayIds.timerMinuteSetPoint);
}

static void decMinuteButtonFunction(void)
{
    if (page2Control.setPointTimer.hours == 0U)
    {
        if (page2Control.setPointTimer.minutes == 1U)
        {
            page2Control.setPointTimer.minutes = 59U;
        }
        else
        {
            page2Control.setPointTimer.minutes--;
        }
    }
    else
    {
        if (page2Control.setPointTimer.minutes == 0U)
        {
            page2Control.setPointTimer.minutes = 59U;
        }
        else
        {
            page2Control.setPointTimer.minutes--;
        }
    }
          
  ili9341Display_Draw(page2Control.displayIds.timerMinuteSetPoint);
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

static uint8_t initHourSetPointDisplay(void)
{
    ILI9341_Display_t timerHourSetPointDisplay;
    
    timerHourSetPointDisplay.x = 0;
    timerHourSetPointDisplay.y = 120;
    timerHourSetPointDisplay.width = 25;
    timerHourSetPointDisplay.height = 30;
    timerHourSetPointDisplay.background = ILI9341_YELLOW;
    timerHourSetPointDisplay.variable = &page2Control.setPointTimer.hours;
    timerHourSetPointDisplay.font = &Font_11x18;
    timerHourSetPointDisplay.color = ILI9341_BLACK;
    
    return ili9341Display_Add(&timerHourSetPointDisplay);  
}

static uint8_t initMinuteSetPointDisplay(void)
{
    ILI9341_Display_t timerMinuteSetPointDisplay;
    
    timerMinuteSetPointDisplay.x = 30;
    timerMinuteSetPointDisplay.y = 120;
    timerMinuteSetPointDisplay.width = 25;
    timerMinuteSetPointDisplay.height = 30;
    timerMinuteSetPointDisplay.background = ILI9341_YELLOW;
    timerMinuteSetPointDisplay.variable = &page2Control.setPointTimer.minutes;
    timerMinuteSetPointDisplay.font = &Font_11x18;
    timerMinuteSetPointDisplay.color = ILI9341_BLACK;
    
    return ili9341Display_Add(&timerMinuteSetPointDisplay);  
}

static uint8_t initHourIsPointDisplay(void)
{
    ILI9341_Display_t timerHourIsPointDisplay;
    
    timerHourIsPointDisplay.x = 0;
    timerHourIsPointDisplay.y = 180;
    timerHourIsPointDisplay.width = 25;
    timerHourIsPointDisplay.height = 30;
    timerHourIsPointDisplay.background = ILI9341_YELLOW;
    timerHourIsPointDisplay.variable = &page2Control.isPointTimer.hours;
    timerHourIsPointDisplay.font = &Font_11x18;
    timerHourIsPointDisplay.color = ILI9341_BLACK;
    
    return ili9341Display_Add(&timerHourIsPointDisplay);  
}

static uint8_t initMinuteIsPointDisplay(void)
{
    ILI9341_Display_t timerMinutePointDisplay;
    
    timerMinutePointDisplay.x = 30;
    timerMinutePointDisplay.y = 180;
    timerMinutePointDisplay.width = 25;
    timerMinutePointDisplay.height = 30;
    timerMinutePointDisplay.background = ILI9341_YELLOW;
    timerMinutePointDisplay.variable = &page2Control.isPointTimer.minutes;
    timerMinutePointDisplay.font = &Font_11x18;
    timerMinutePointDisplay.color = ILI9341_BLACK;
    
    return ili9341Display_Add(&timerMinutePointDisplay);  
}

static uint8_t initSecondIsPointDisplay(void)
{
    ILI9341_Display_t timerSecondPointDisplay;
    
    timerSecondPointDisplay.x = 60;
    timerSecondPointDisplay.y = 180;
    timerSecondPointDisplay.width = 25;
    timerSecondPointDisplay.height = 30;
    timerSecondPointDisplay.background = ILI9341_YELLOW;
    timerSecondPointDisplay.variable = &page2Control.isPointTimer.seconds;
    timerSecondPointDisplay.font = &Font_11x18;
    timerSecondPointDisplay.color = ILI9341_BLACK;
    
    return ili9341Display_Add(&timerSecondPointDisplay);  
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

static void controlShootCamera(void)
{
    uint32_t setPointSeconds = 0U;
    if (true == page2Control.continuousTriggerEnabled)
    {
        if (Timebase_isTimeout(page2Control.timer))
        {
            setPointSeconds = convertHoursMinutesSecondsToSeconds(page2Control.setPointTimer);
            /*TODO: after that the field for the timer is available, the one will be in the parameter as timebase_getValue*/
            page2Control.timer = Timebase_getValue(setPointSeconds*10);
            CameraControl_triggerManualPicture();
        }
    }
    else
    {
        page2Control.timer = 0U;
    }
}

static void controlIsTimerDisplay(void)
{
    uint32_t differenceTimer = Timebase_getDifference(page2Control.timer);
    
    differenceTimer /= 10;
    
    if (differenceTimer != page2Control.isPointSeconds)
    {
        page2Control.isPointSeconds = differenceTimer;
        
        convertSecondsToHoursMinutesSeconds(differenceTimer,&page2Control.isPointTimer);
        ili9341Display_Draw(page2Control.displayIds.timerHourIsPoint);
        ili9341Display_Draw(page2Control.displayIds.timerMinuteIsPoint);
        ili9341Display_Draw(page2Control.displayIds.timerSecondIsPoint);
        /*TODO: to draw the fields for the is point timer*/
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
