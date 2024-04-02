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
#define  TIMEOUT_30_SEC    (ONE_SECOND_TIMER*30U)
#define  TIMEOUT_2_MIN    (ONE_SECOND_TIMER*60U*2)
   
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
    bool continuousTriggerEnabled;
    bool changeEnableDisableContinous;
    uint32_t setPointSeconds;
    uint32_t timer;
    buttonTouchIds_t buttonTouchIds; 
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
static void initDisplay1(void);
   
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
            page2Control.timer = Timebase_getValue(TIMEOUT_2_MIN);
            CameraControl_triggerManualPicture();
        }
    }   
}

void Page2_showPage(void)
{
    memset(&page2Control.buttonTouchIds,0x00,sizeof(buttonTouchIds_t));
    page2Control.continuousTriggerEnabled = false;
    page2Control.changeEnableDisableContinous = true;
    page2Control.timer = 0U;
    ili9341Button_DeleteAll();
    
    
    page2Control.buttonTouchIds.manualId = initManualButton();
    page2Control.buttonTouchIds.sensorId = initSensorButton();
    page2Control.buttonTouchIds.disableId = initDisableButton();
    page2Control.buttonTouchIds.enableId = initEnableButton();
      
    
    /*TM_ILI9341_Button_t buttonTest;
    
    buttonTest.x = 50;
    buttonTest.y = 50;
    buttonTest.width = 60;
    buttonTest.height = 30;
    buttonTest.background = ILI9341_YELLOW;
    buttonTest.pressedBackground = ILI9341_RED;
    buttonTest.label = "Picture";
    buttonTest.font = &Font_11x18;
    buttonTest.color = ILI9341_BLACK;
    buttonTest.callbackFunction = buttonTestFunction;
    
    
    
    
    buttonTestId = ili9341Button_Add(&buttonTest);*/
    
    ILI9341_FillScreen(ILI9341_BLACK);
    
    initDisplay1();
    
    ili9341Button_Draw(page2Control.buttonTouchIds.manualId);
    ili9341Button_Draw(page2Control.buttonTouchIds.sensorId);
    ili9341Button_Draw(page2Control.buttonTouchIds.disableId);
    ili9341Button_Draw(page2Control.buttonTouchIds.enableId);
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

static uint8_t initIncTimeButton(void);
static uint8_t initDecTimeButton(void);

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

static void initDisplay1(void)
{
    ILI9341_Display_t testDisplay;
    
    testDisplay.x = 50;
    testDisplay.y = 75;
    testDisplay.width = 60;
    testDisplay.height = 30;
    testDisplay.background = ILI9341_YELLOW;
    testDisplay.variable = &testVariable;
    testDisplay.font = &Font_11x18;
    testDisplay.color = ILI9341_BLACK;
    
    numberDisplay = ili9341Display_Add(&testDisplay);
    (void)ili9341Display_Draw(numberDisplay);
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
