/*
 * Page2.c
 *
 *  Created on: januar 2024
 *      Author: Alejandro Bauernfeind
 */
#include "Application/Visual/Pages/Page2.h"

#include "Application/Camera/output/CameraControl.h"
#include "Application/Visual/Tools/ili9341Button.h"
#include "Application/Visual/Tools/ili9341Display.h"
#include "Service/TouchPanel/Panel/ili9341.h"
#include "Service/TouchPanel/Touch/ili9341Touch.h"

static PagesTypes_t nextPage;
static uint8_t buttonTestId = 0;
static uint32_t testVariable = 20;
static uint8_t numberDisplay; 

static void buttonTestFunction(void);
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
            ILI9341_FillCircle(touchData.x, touchData.y, 2, ILI9341_YELLOW);
        }
        //nextPage = PAGES_NUMBER_1;
    }
    else
    {
        (void)ili9341Button_Touch(&touchData);
    }
}

void Page2_showPage(void)
{
    TM_ILI9341_Button_t buttonTest;
    
    buttonTest.x = 50;
    buttonTest.y = 50;
    buttonTest.width = 60;
    buttonTest.height = 30;
    buttonTest.background = ILI9341_YELLOW;
    buttonTest.pressedBackground = ILI9341_RED;
    buttonTest.label = "test";
    buttonTest.font = &Font_11x18;
    buttonTest.color = ILI9341_BLACK;
    buttonTest.callbackFunction = buttonTestFunction;
    
    
    buttonTestId = ili9341Button_Add(&buttonTest);
    
    ILI9341_FillScreen(ILI9341_BLACK);
    ILI9341_WriteString(0,3*10,"Camera page2",Font_11x18,ILI9341_GREEN,ILI9341_YELLOW);
    
    initDisplay1();
    ili9341Button_Draw(buttonTestId);

        
    nextPage = PAGES_NUMBER_2;
}

PagesTypes_t Page2_nextPage(void)
{
    return nextPage;
}

/*------INTERNAL VARIABLES------------*/

static void buttonTestFunction(void)
{
    //TM_ILI9341_Button_DeleteAll();
    //nextPage = PAGES_NUMBER_1;
    testVariable += 50;
    (void)ili9341Display_Draw(numberDisplay);
    CameraControl_triggerManualPicture();
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
