
#include "Application/Visual/Tools/ili9341Display.h"

#include "Service/TouchPanel/Panel/ili9341.h"
#include <stdio.h>
#include <stdlib.h>

static void intToString(uint32_t num, char *str);

ILI9341_Display_t ILI9341_Displays[ILI9341_DISPLAY_MAX_DISPLAYS];

int8_t ili9341Display_Add(ILI9341_Display_t* display) 
{
	uint8_t id = 0;
	while ((ILI9341_Displays[id].used) && (id < ILI9341_DISPLAY_MAX_DISPLAYS)) 
        {
		id++;
	}
	
	if (id >= ILI9341_DISPLAY_MAX_DISPLAYS) 
        {
		return -1;
	}
        else
        {
            ILI9341_Displays[id].used = true;
        }
	ILI9341_Displays[id].x = display->x;
	ILI9341_Displays[id].y = display->y;
	ILI9341_Displays[id].width = display->width;
	ILI9341_Displays[id].height = display->height;
	ILI9341_Displays[id].background = display->background;
	ILI9341_Displays[id].borderColor = display->borderColor;
        if(display->variable != NULL)
        {
            ILI9341_Displays[id].variable = display->variable;
        }
        ILI9341_Displays[id].font = display->font;
	ILI9341_Displays[id].color = display->color;

	return id;
}

void ili9341Display_DrawAll(void) 
{
	uint8_t id = 0;
	for (id = 0; id < ILI9341_DISPLAY_MAX_DISPLAYS; id++) 
        {
		if ((ILI9341_Displays[id].used)) 
                {
			ili9341Display_Draw(id);
		}
	}
}

DispErrorStatus_t ili9341Display_Draw(uint8_t id) 
{
	uint16_t fontWidth, fontHeight, x, y;
        char strVariable[5];
	if ((ILI9341_Displays[id].used) == false) 
        {
		return DISP_ERROR;
	}

        //Background
        ILI9341_FillRoundRect(	ILI9341_Displays[id].x,
                                ILI9341_Displays[id].y,
                                ILI9341_Displays[id].width,
                                ILI9341_Displays[id].height,
                                5,
                                ILI9341_Displays[id].background );
        
         //Border enabled
        ILI9341_DrawRect( ILI9341_Displays[id].x,
                          ILI9341_Displays[id].y,
                          ILI9341_Displays[id].width,
                          ILI9341_Displays[id].height,
                          ILI9341_Displays[id].borderColor );
		
	//Display label
	if (ILI9341_Displays[id].variable != NULL) 
        {
                intToString(*ILI9341_Displays[id].variable,strVariable);
		fontWidth = ILI9341_Displays[id].font->width;
                fontHeight = ILI9341_Displays[id].font->height;
		x = ILI9341_Displays[id].x + ILI9341_Displays[id].width / 2 - (fontWidth*2);
		y = ILI9341_Displays[id].y + ILI9341_Displays[id].height / 2 - (fontHeight / 2);
		ILI9341_WriteString(x, y, strVariable, *ILI9341_Displays[id].font, ILI9341_Displays[id].color, ILI9341_Displays[id].background);
	}
	

	return DISP_SUCCESS;
}


void ili9341Display_DeleteAll(void) {
	uint8_t i;
	for (i = 0; i < ILI9341_DISPLAY_MAX_DISPLAYS; i++) {
		ili9341Display_Delete(i);
	}
}

void ili9341Display_Delete(uint8_t id) 
{
	ILI9341_Displays[id].used = false;
}


/*------ INTERNAL FUNCTIONS ----------------------*/
static void intToString(uint32_t num, char *str) 
{
    sprintf(str, "%u", num);
}

