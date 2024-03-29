/*
 * Page1.c
 *
 *  Created on: januar 2024
 *      Author: Alejandro Bauernfeind
 */
#include "Application/Visual/Pages/Page1.h"

#include "Application/Visual/Pages/Page2.h"
#include "Service/TouchPanel/Panel/ili9341.h"
#include "Service/TouchPanel/Touch/ili9341Touch.h"

static PagesTypes_t nextPage;
   
void Page1_execute(void)
{
    if (false == ILI9341Touch_isPressed())
    {
        nextPage = PAGES_NUMBER_2;
    }
}

void Page1_showPage(void)
{
    ILI9341_FillScreen(ILI9341_BLACK);
    ILI9341_WriteString(0,3*10,"Camera Intro",Font_11x18,ILI9341_GREEN,ILI9341_YELLOW);
    nextPage = PAGES_NUMBER_1;
}

PagesTypes_t Page1_nextPage(void)
{
    return nextPage;
}
