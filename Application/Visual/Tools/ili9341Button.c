
#include "Application/Visual/Tools/ili9341Button.h"

#include "Service/TouchPanel/Panel/ili9341.h"

static bool isTouchDataInsideButton(TouchData_t* touchData, uint8_t id);
static void drawButtonWithBkgPressed(uint8_t id);

TM_ILI9341_Button_t TM_ILI9341_Buttons[TM_ILI9341_BUTTON_MAX_BUTTONS];

int8_t ili9341Button_Add(TM_ILI9341_Button_t* button) 
{
	uint8_t id = 0;
	while ((TM_ILI9341_Buttons[id].flags & TM_BUTTON_FLAG_USED) && (id < TM_ILI9341_BUTTON_MAX_BUTTONS)) 
        {
		id++;
	}
	
	if (id == TM_ILI9341_BUTTON_MAX_BUTTONS) 
        {
		//Max button reached
		return -1;
	}
	TM_ILI9341_Buttons[id].x = button->x;
	TM_ILI9341_Buttons[id].y = button->y;
	TM_ILI9341_Buttons[id].width = button->width;
	TM_ILI9341_Buttons[id].height = button->height;
	TM_ILI9341_Buttons[id].background = button->background;
        TM_ILI9341_Buttons[id].pressedBackground = button->pressedBackground;
	TM_ILI9341_Buttons[id].borderColor = button->borderColor;
	TM_ILI9341_Buttons[id].flags = button->flags | TM_BUTTON_FLAG_USED | TM_BUTTON_FLAG_ENABLED;
	TM_ILI9341_Buttons[id].label = button->label;
	TM_ILI9341_Buttons[id].color = button->color;
	TM_ILI9341_Buttons[id].font = button->font;
	TM_ILI9341_Buttons[id].image = button->image;
        TM_ILI9341_Buttons[id].callbackFunction = button->callbackFunction;
        TM_ILI9341_Buttons[id].wasButtonPressed = false;
	
	return id;
}

void ili9341Button_DrawAll(void) 
{
	uint8_t id = 0;
	for (id = 0; id < TM_ILI9341_BUTTON_MAX_BUTTONS; id++) 
        {
		if ((TM_ILI9341_Buttons[id].flags & TM_BUTTON_FLAG_USED)) 
                {
			//Button enabled, draw it to screen
			ili9341Button_Draw(id);
		}
	}
}

ErrorStatus_t ili9341Button_Draw(uint8_t id) 
{
	uint16_t fontWidth, fontHeight, x, y, i, j;
	if ((TM_ILI9341_Buttons[id].flags & TM_BUTTON_FLAG_USED) == 0) 
        {
		//Button not enabled
		return ERROR;
	}
	//Draw label
	if (TM_ILI9341_Buttons[id].flags & TM_BUTTON_FLAG_IMAGE) 
        {
		//Draw picture
		for (i = 0; i < TM_ILI9341_Buttons[id].width; i++) 
                {
			for (j = 0; j < TM_ILI9341_Buttons[id].height; j++) 
                        {
				ILI9341_DrawPixel(TM_ILI9341_Buttons[id].x + i,
                                                  TM_ILI9341_Buttons[id].y + j,
                                                  *(uint16_t *)(TM_ILI9341_Buttons[id].image + j * TM_ILI9341_Buttons[id].width + i));
			}
		}
	} 
        else 
        {
		//Background
		ILI9341_FillRoundRect(	TM_ILI9341_Buttons[id].x,
                                        TM_ILI9341_Buttons[id].y,
                                        TM_ILI9341_Buttons[id].width,
                                        TM_ILI9341_Buttons[id].height,
                                        5,
                                        TM_ILI9341_Buttons[id].background );
		
	}
	
	//Display label
	if ((TM_ILI9341_Buttons[id].flags & TM_BUTTON_FLAG_NOLABEL) == 0) 
        {
		//TM_ILI9341_GetStringSize(TM_ILI9341_Buttons[id].label, TM_ILI9341_Buttons[id].font, &fontWidth, &fontHeight);
                fontWidth = TM_ILI9341_Buttons[id].font->width;
                fontHeight = TM_ILI9341_Buttons[id].font->height;
		x = TM_ILI9341_Buttons[id].x + TM_ILI9341_Buttons[id].width / 2 - (fontWidth*2);
		y = TM_ILI9341_Buttons[id].y + TM_ILI9341_Buttons[id].height / 2 - (fontHeight / 2);
		ILI9341_WriteString(x, y, TM_ILI9341_Buttons[id].label, *TM_ILI9341_Buttons[id].font, TM_ILI9341_Buttons[id].color, TM_ILI9341_Buttons[id].background);
	}
	
	//Border
	if ((TM_ILI9341_Buttons[id].flags & TM_BUTTON_FLAG_NOBORDER) == 0) 
        {
		//Border enabled
		ILI9341_DrawRect( TM_ILI9341_Buttons[id].x,
                                  TM_ILI9341_Buttons[id].y,
                                  TM_ILI9341_Buttons[id].width,
                                  TM_ILI9341_Buttons[id].height,
                                  TM_ILI9341_Buttons[id].borderColor );
	}
	
	return SUCCESS;
}

int8_t ili9341Button_Touch(TouchData_t* touchData) 
{
	uint8_t id;
	for (id = 0; id < TM_ILI9341_BUTTON_MAX_BUTTONS; id++) 
        {
		//If button not enabled, ignore it
		if ((TM_ILI9341_Buttons[id].flags & TM_BUTTON_FLAG_ENABLED) == 0) 
                {
			continue;
		}
		//If touch data is inside button somewhere
                if(isTouchDataInsideButton(touchData,id))
		{
                    if(TM_ILI9341_Buttons[id].wasButtonPressed == false)
                    {
                        TM_ILI9341_Buttons[id].wasButtonPressed = true;
                        drawButtonWithBkgPressed(id);
                    }
                    return id;
		}
                else
                {
                    if(TM_ILI9341_Buttons[id].wasButtonPressed == true)
                    {
                        TM_ILI9341_Buttons[id].wasButtonPressed = false;
                        //Calls the callback function
                        if(TM_ILI9341_Buttons[id].callbackFunction != NULL)
                        {
                            TM_ILI9341_Buttons[id].callbackFunction();
                            //Return its id
                        }
                        ili9341Button_Draw(id);
                    }
                }               
	}
	
	//No one was pressed
	return -1;
}


void ili9341Button_Enable(uint8_t id) {
	//Add enabled flag
	TM_ILI9341_Buttons[id].flags |= TM_BUTTON_FLAG_ENABLED;
}

void ili9341Button_Disable(uint8_t id) {
	//Remove enabled flag
	TM_ILI9341_Buttons[id].flags &= ~TM_BUTTON_FLAG_ENABLED;
}

void ili9341Button_DeleteAll(void) {
	uint8_t i;
	for (i = 0; i < TM_ILI9341_BUTTON_MAX_BUTTONS; i++) {
		ili9341Button_Delete(i);
	}
}

void ili9341Button_Delete(uint8_t id) {
	//Just remove USED flag from button
	TM_ILI9341_Buttons[id].flags &= ~TM_BUTTON_FLAG_USED;
}

static bool isTouchDataInsideButton(TouchData_t* touchData, uint8_t id)
{
    bool retVal = false;
  
    bool isBiggerThanX = touchData->x > TM_ILI9341_Buttons[id].x;
    bool isSmallerThanXPlusWidth = touchData->x < (TM_ILI9341_Buttons[id].x + TM_ILI9341_Buttons[id].width);
    bool isBiggerThanY = touchData->y > TM_ILI9341_Buttons[id].y;
    bool isSmallerThanYPlusHeight = touchData->y < (TM_ILI9341_Buttons[id].y + TM_ILI9341_Buttons[id].height);
    
    if (isBiggerThanX && isSmallerThanXPlusWidth && isBiggerThanY && isSmallerThanYPlusHeight)
    {
        retVal = true;
    }
    
    return retVal;
}

static void drawButtonWithBkgPressed(uint8_t id)
{
    uint16_t fontWidth, fontHeight, x, y;
    if(TM_ILI9341_Buttons[id].pressedBackground!=0)
    {
            ILI9341_FillRoundRect(TM_ILI9341_Buttons[id].x,
                        TM_ILI9341_Buttons[id].y,
                        TM_ILI9341_Buttons[id].width,
                        TM_ILI9341_Buttons[id].height,
                        5,
                        TM_ILI9341_Buttons[id].pressedBackground );
    }
    
    //Display label
    if ((TM_ILI9341_Buttons[id].flags & TM_BUTTON_FLAG_NOLABEL) == 0) 
    {
            //TM_ILI9341_GetStringSize(TM_ILI9341_Buttons[id].label, TM_ILI9341_Buttons[id].font, &fontWidth, &fontHeight);
            fontWidth = TM_ILI9341_Buttons[id].font->width;
            fontHeight = TM_ILI9341_Buttons[id].font->height;
            x = TM_ILI9341_Buttons[id].x + TM_ILI9341_Buttons[id].width / 2 - (fontWidth*2);
            y = TM_ILI9341_Buttons[id].y + TM_ILI9341_Buttons[id].height / 2 - (fontHeight / 2);
            ILI9341_WriteString(x, y, TM_ILI9341_Buttons[id].label, *TM_ILI9341_Buttons[id].font, TM_ILI9341_Buttons[id].color, TM_ILI9341_Buttons[id].pressedBackground);
    }
    
    //Border
    if ((TM_ILI9341_Buttons[id].flags & TM_BUTTON_FLAG_NOBORDER) == 0) 
    {
            //Border enabled
            ILI9341_DrawRect( TM_ILI9341_Buttons[id].x,
                              TM_ILI9341_Buttons[id].y,
                              TM_ILI9341_Buttons[id].width,
                              TM_ILI9341_Buttons[id].height,
                              TM_ILI9341_Buttons[id].borderColor );
    }
}

