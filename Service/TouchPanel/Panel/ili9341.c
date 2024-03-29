/*
 * ili9341.c
 *
 *  Created on: januar 2024
 *      Author: Alejandro Bauernfeind
 */
#include "Service/TouchPanel/Panel/ili9341.h"
   
#include <stdlib.h>
#include "Service/Delay/Delay.h"
#include "HAL/GPIO/GpioPortA.h"
#include "HAL/SPI/HalSpi.h"
   
#define ILI9341_CS_Pin (1U << 3U)
#define ILI9341_SPI_OUT (1U << 5U)
#define ILI9341_DC_Pin (1U << 6U)
#define ILI9341_RES_Pin (1U << 7U)


//Comandos para el ili9341
#define CMD_READ_DISPLAY_STATUS 0x09
#define CMD_READ_DISPLAY_ID_INFO 0x04
#define CMD_READ_DISPLAY_PWR_MODE 0x0A

#define SWAP_INT16_T(a, b) { int16_t t = a; a = b; b = t; }
#define ABS(x) ((x) < 0 ? -(x) : (x))


#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04



static void ILI9341_WriteCommand(uint8_t cmd);
static void ILI9341_WriteData(uint8_t* buff, uint32_t buffSize);
static void ILI9341_ReadData(void);
static void ILI9341_Unselect();
static void ILI9341_Select(); 
static void ILI9341_Reset();
static void ILI9341_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
static void ILI9341_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color); 



void ILI9341_FillScreen(uint16_t color) 
{
    ILI9341_Select();
    ILI9341_FillRectangle(0, 0, ILI9341_WIDTH, ILI9341_HEIGHT, color);
    ILI9341_Unselect();
}

void ILI9341_Init() {
    ILI9341_Select();
    ILI9341_Reset();

    // command list is based on https://github.com/martnak/STM32-ILI9341

    // SOFTWARE RESET
    ILI9341_WriteCommand(0x01);
    Delay_ms(100);
    
    //ILI9341_WriteCommand(0xDA); //id del ili
    //ILI9341_ReadData();
    
   

    // POWER CONTROL A
    ILI9341_WriteCommand(0xCB);
    {
        uint8_t data[] = { 0x39, 0x2C, 0x00, 0x34, 0x02 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // POWER CONTROL B
    ILI9341_WriteCommand(0xCF);
    {
        uint8_t data[] = { 0x00, 0xC1, 0x30 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // DRIVER TIMING CONTROL A
    ILI9341_WriteCommand(0xE8);
    {
        uint8_t data[] = { 0x85, 0x00, 0x78 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // DRIVER TIMING CONTROL B
    ILI9341_WriteCommand(0xEA);
    {
        uint8_t data[] = { 0x00, 0x00 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // POWER ON SEQUENCE CONTROL
    ILI9341_WriteCommand(0xED);
    {
        uint8_t data[] = { 0x64, 0x03, 0x12, 0x81 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // PUMP RATIO CONTROL
    ILI9341_WriteCommand(0xF7);
    {
        uint8_t data[] = { 0x20 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // POWER CONTROL,VRH[5:0]
    ILI9341_WriteCommand(0xC0);
    {
        uint8_t data[] = { 0x23 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // POWER CONTROL,SAP[2:0];BT[3:0]
    ILI9341_WriteCommand(0xC1);
    {
        uint8_t data[] = { 0x10 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // VCM CONTROL
    ILI9341_WriteCommand(0xC5);
    {
        uint8_t data[] = { 0x3E, 0x28 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // VCM CONTROL 2
    ILI9341_WriteCommand(0xC7);
    {
        uint8_t data[] = { 0x86 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // MEMORY ACCESS CONTROL
    ILI9341_WriteCommand(0x36);
    {
        uint8_t data[] = { 0x48 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // PIXEL FORMAT
    ILI9341_WriteCommand(0x3A);
    {
        uint8_t data[] = { 0x55 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // FRAME RATIO CONTROL, STANDARD RGB COLOR
    ILI9341_WriteCommand(0xB1);
    {
        uint8_t data[] = { 0x00, 0x18 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // DISPLAY FUNCTION CONTROL
    ILI9341_WriteCommand(0xB6);
    {
        uint8_t data[] = { 0x08, 0x82, 0x27 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // 3GAMMA FUNCTION DISABLE
    ILI9341_WriteCommand(0xF2);
    {
        uint8_t data[] = { 0x00 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // GAMMA CURVE SELECTED
    ILI9341_WriteCommand(0x26);
    {
        uint8_t data[] = { 0x01 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // POSITIVE GAMMA CORRECTION
    ILI9341_WriteCommand(0xE0);
    {
        uint8_t data[] = { 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
                           0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // NEGATIVE GAMMA CORRECTION
    ILI9341_WriteCommand(0xE1);
    {
        uint8_t data[] = { 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
                           0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F };
        ILI9341_WriteData(data, sizeof(data));
    }

    // EXIT SLEEP
    ILI9341_WriteCommand(0x11);
    Delay_ms(120);

    // TURN ON DISPLAY
    ILI9341_WriteCommand(0x29);

    // MADCTL
    ILI9341_WriteCommand(0x36);
    {
        uint8_t data[] = { ILI9341_ROTATION };
        ILI9341_WriteData(data, sizeof(data));
    }

    ILI9341_Unselect();
}

void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color) 
{
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT))
        return;

    ILI9341_Select();

    ILI9341_SetAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = { color >> 8, color & 0xFF };
    ILI9341_WriteData(data, sizeof(data));

    ILI9341_Unselect();
}

void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) 
{
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
    if((x + w - 1) >= ILI9341_WIDTH) return;
    if((y + h - 1) >= ILI9341_HEIGHT) return;

    ILI9341_Select();
    ILI9341_SetAddressWindow(x, y, x+w-1, y+h-1);
    ILI9341_WriteData((uint8_t*)data, sizeof(uint16_t)*w*h);
    ILI9341_Unselect();
}

void ILI9341_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) {
    ILI9341_Select();

    while(*str) {
        if(x + font.width >= ILI9341_WIDTH) {
            x = 0;
            y += font.height;
            if(y + font.height >= ILI9341_HEIGHT) {
                break;
            }

            if(*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        ILI9341_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }

    ILI9341_Unselect();
}

void ILI9341_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) 
{
    uint32_t i, b, j;

    ILI9341_SetAddressWindow(x, y, x+font.width-1, y+font.height-1);

    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                uint8_t data[] = { color >> 8, color & 0xFF };
                ILI9341_WriteData(data, sizeof(data));
            } else {
                uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                ILI9341_WriteData(data, sizeof(data));
            }
        }
    }
}

void ILI9341_InvertColors(bool invert) 
{
    ILI9341_Select();
    ILI9341_WriteCommand(invert ? 0x21 /* INVON */ : 0x20 /* INVOFF */);
    ILI9341_Unselect();
}

/***************************************************************************************
** Function name:           drawCircle
** Description:             Draw a circle outline
***************************************************************************************/
void ILI9341_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = - r - r;
  int16_t x = 0;
  
  ILI9341_Select();

  ILI9341_DrawPixel(x0 + r, y0  , color);
  ILI9341_DrawPixel(x0 - r, y0  , color);
  ILI9341_DrawPixel(x0  , y0 - r, color);
  ILI9341_DrawPixel(x0  , y0 + r, color);

  while (x < r)
  {
    if (f >= 0)
    {
      r--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    ILI9341_DrawPixel(x0 + x, y0 + r, color);
    ILI9341_DrawPixel(x0 - x, y0 + r, color);
    ILI9341_DrawPixel(x0 - x, y0 - r, color);
    ILI9341_DrawPixel(x0 + x, y0 - r, color);

    ILI9341_DrawPixel(x0 + r, y0 + x, color);
    ILI9341_DrawPixel(x0 - r, y0 + x, color);
    ILI9341_DrawPixel(x0 - r, y0 - x, color);
    ILI9341_DrawPixel(x0 + r, y0 - x, color);
  }
}


void ILI9341_DrawTouchGFX(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data)
{
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
    if((x + w - 1) >= ILI9341_WIDTH) return;
    if((y + h - 1) >= ILI9341_HEIGHT) return;

    ILI9341_SetAddressWindow(x, y, x+w-1, y+h-1);

    uint32_t size = w * h;
    //uint8_t colorBytes[size][2];
    
    uint8_t (*colorBytes)[2] = malloc(size * sizeof(*colorBytes));

    for (uint32_t i = 0; i < size; i++)
      {
        colorBytes[i][0] = (*data & 0xFF00) >> 8;
        colorBytes[i][1] = *data & 0x00FF;
        data++;
      }

    ILI9341_Select();
    ILI9341_WriteData((uint8_t*) &colorBytes, size * 2);
    ILI9341_Unselect();
}


/***************************************************************************************
** Function name:           drawCircleHelper
** Description:             Support function for circle drawing
***************************************************************************************/
void ILI9341_DrawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;

  while (x < r)
  {
    if (f >= 0)
    {
      r--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x8)
    {
      ILI9341_DrawPixel(x0 - r, y0 + x, color);
      ILI9341_DrawPixel(x0 - x, y0 + r, color);
    }
    if (cornername & 0x4)
    {
      ILI9341_DrawPixel(x0 + x, y0 + r, color);
      ILI9341_DrawPixel(x0 + r, y0 + x, color);
    }
    if (cornername & 0x2)
    {
      ILI9341_DrawPixel(x0 + r, y0 - x, color);
      ILI9341_DrawPixel(x0 + x, y0 - r, color);
    }
    if (cornername & 0x1)
    {
      ILI9341_DrawPixel(x0 - x, y0 - r, color);
      ILI9341_DrawPixel(x0 - r, y0 - x, color);
    }

  }
}

/***************************************************************************************
** Function name:           fillCircle
** Description:             draw a filled circle
***************************************************************************************/
void ILI9341_FillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  ILI9341_DrawFastVLine(x0, y0 - r, r + r + 1, color);
  ILI9341_FillCircleHelper(x0, y0, r, 3, 0, color);
}

/***************************************************************************************
** Function name:           fillCircleHelper
** Description:             Support function for filled circle drawing
***************************************************************************************/
// Used to do circles and roundrects
void ILI9341_FillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color)
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -r - r;
  int16_t x     = 0;

  delta++;
  while (x < r)
  {
    if (f >= 0)
    {
      r--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1)
    {
      ILI9341_DrawFastVLine(x0 + x, y0 - r, r + r + delta, color);
      ILI9341_DrawFastVLine(x0 + r, y0 - x, x + x + delta, color);
    }
    if (cornername & 0x2)
    {
      ILI9341_DrawFastVLine(x0 - x, y0 - r, r + r + delta, color);
      ILI9341_DrawFastVLine(x0 - r, y0 - x, x + x + delta, color);
    }
  }
}

/***************************************************************************************
** Function name:           drawEllipse
** Description:             Draw a ellipse outline
***************************************************************************************/
void ILI9341_DrawEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color)
{
  if (rx < 2) return;
  if (ry < 2) return;
  int16_t x, y;
  int32_t rx2 = rx * rx;
  int32_t ry2 = ry * ry;
  int32_t fx2 = 4 * rx2;
  int32_t fy2 = 4 * ry2;
  int32_t s;

  for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1-2 * ry); ry2 * x <= rx2 * y; x++)
  {
    ILI9341_DrawPixel(x0 + x, y0 + y, color);
    ILI9341_DrawPixel(x0 - x, y0 + y, color);
    ILI9341_DrawPixel(x0 - x, y0 - y, color);
    ILI9341_DrawPixel(x0 + x, y0 - y, color);
    if (s >= 0)
    {
      s += fx2 * (1 - y);
      y--;
    }
    s += ry2 * ((4 * x) + 6);
  }

  for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++)
  {
    ILI9341_DrawPixel(x0 + x, y0 + y, color);
    ILI9341_DrawPixel(x0 - x, y0 + y, color);
    ILI9341_DrawPixel(x0 - x, y0 - y, color);
    ILI9341_DrawPixel(x0 + x, y0 - y, color);
  if (s >= 0)
  {
    s += fy2 * (1 - x);
    x--;
  }
  s += rx2 * ((4 * y) + 6);
  }
}

/***************************************************************************************
** Function name:           fillEllipse
** Description:             draw a filled ellipse
***************************************************************************************/
void ILI9341_FillEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color)
{
  if (rx < 2) return;
  if (ry < 2) return;
  int16_t x, y;
  int32_t rx2 = rx * rx;
  int32_t ry2 = ry * ry;
  int32_t fx2 = 4 * rx2;
  int32_t fy2 = 4 * ry2;
  int32_t s;

  for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++)
  {
    ILI9341_DrawFastHLine(x0 - x, y0 - y, x + x + 1, color);
    ILI9341_DrawFastHLine(x0 - x, y0 + y, x + x + 1, color);

    if (s >= 0)
    {
      s += fx2 * (1 - y);
      y--;
    }
    s += ry2 * ((4 * x) + 6);
  }

  for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++)
  {
    ILI9341_DrawFastHLine(x0 - x, y0 - y, x + x + 1, color);
    ILI9341_DrawFastHLine(x0 - x, y0 + y, x + x + 1, color);

    if (s >= 0)
    {
      s += fy2 * (1 - x);
      x--;
    }
    s += rx2 * ((4 * y) + 6);
  }

}

/***************************************************************************************
** Function name:           drawRect
** Description:             Draw a rectangle outline
***************************************************************************************/
// Draw a rectangle
void ILI9341_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  ILI9341_DrawFastHLine(x, y, w, color);
  ILI9341_DrawFastHLine(x, y + h - 1, w, color);
  ILI9341_DrawFastVLine(x, y, h, color);
  ILI9341_DrawFastVLine(x + w - 1, y, h, color);
}

/***************************************************************************************
** Function name:           drawRoundRect
** Description:             Draw a rounded corner rectangle outline
***************************************************************************************/
// Draw a rounded rectangle
void ILI9341_DrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  // smarter version
  ILI9341_DrawFastHLine(x + r  , y    , w - r - r, color); // Top
  ILI9341_DrawFastHLine(x + r  , y + h - 1, w - r - r, color); // Bottom
  ILI9341_DrawFastVLine(x    , y + r  , h - r - r, color); // Left
  ILI9341_DrawFastVLine(x + w - 1, y + r  , h - r - r, color); // Right
  // draw four corners
  ILI9341_DrawCircleHelper(x + r    , y + r    , r, 1, color);
  ILI9341_DrawCircleHelper(x + r    , y + h - r - 1, r, 8, color);
  ILI9341_DrawCircleHelper(x + w - r - 1, y + r    , r, 2, color);
  ILI9341_DrawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
}

/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
// Fill a rounded rectangle
void ILI9341_FillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  // smarter version
  ILI9341_FillRectangle(x + r, y, w - r - r, h, color);

  // draw four corners
  ILI9341_FillCircleHelper(x + w - r - 1, y + r, r, 1, h - r - r - 1, color);
  ILI9341_FillCircleHelper(x + r    , y + r, r, 2, h - r - r - 1, color);
}

/***************************************************************************************
** Function name:           drawTriangle
** Description:             Draw a triangle outline using 3 arbitrary points
***************************************************************************************/
// Draw a triangle
void ILI9341_DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  ILI9341_DrawLine(x0, y0, x1, y1, color);
  ILI9341_DrawLine(x1, y1, x2, y2, color);
  ILI9341_DrawLine(x2, y2, x0, y0, color);
}

/***************************************************************************************
** Function name:           fillTriangle
** Description:             Draw a filled triangle using 3 arbitrary points
***************************************************************************************/
// Fill a triangle - original Adafruit function works well and code footprint is small
void ILI9341_FillTriangle( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1)
  {
    SWAP_INT16_T(y0, y1); SWAP_INT16_T(x0, x1);
  }

  if (y1 > y2)
  {
    SWAP_INT16_T(y2, y1); SWAP_INT16_T(x2, x1);
  }

  if (y0 > y1)
  {
    SWAP_INT16_T(y0, y1); SWAP_INT16_T(x0, x1);
  }

  if (y0 == y2)
  { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if (x1 < a)      a = x1;
    else if (x1 > b) b = x1;
    if (x2 < a)      a = x2;
    else if (x2 > b) b = x2;
    ILI9341_DrawFastHLine(a, y0, b - a + 1, color);
    return;
  }

  int16_t
  dx01 = x1 - x0,
  dy01 = y1 - y0,
  dx02 = x2 - x0,
  dy02 = y2 - y0,
  dx12 = x2 - x1,
  dy12 = y2 - y1,
  sa   = 0,
  sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (y1 == y2) last = y1;  // Include y1 scanline
  else         last = y1 - 1; // Skip it

  for (y = y0; y <= last; y++)
  {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;

    if (a > b) SWAP_INT16_T(a, b);
    ILI9341_DrawFastHLine(a, y, b - a + 1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for (; y <= y2; y++)
  {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;

    if (a > b) SWAP_INT16_T(a, b);
    ILI9341_DrawFastHLine(a, y, b - a + 1, color);
  }
}

/***************************************************************************************
** Function name:           drawLine
** Description:             draw a line between 2 arbitrary points
***************************************************************************************/

// Slower but more compact line drawing function
void ILI9341_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  int16_t steep = ABS(y1 - y0) > ABS(x1 - x0);
  if (steep)
  {
    SWAP_INT16_T(x0, y0);
    SWAP_INT16_T(x1, y1);
  }

  if (x0 > x1)
  {
    SWAP_INT16_T(x0, x1);
    SWAP_INT16_T(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = ABS(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1)
  {
    ystep = 1;
  }
  else
  {
    ystep = -1;
  }

  for (; x0<=x1; x0++)
  {
    if (steep)
    {
      ILI9341_DrawPixel(y0, x0, color);
    }
    else
    {
      ILI9341_DrawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}

/***************************************************************************************
** Function name:           drawFastVLine
** Description:             draw a vertical line
***************************************************************************************/
void ILI9341_DrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  // Rudimentary clipping
  if ((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
  if ((y + h - 1) >= ILI9341_HEIGHT) h = ILI9341_HEIGHT - y;

  ILI9341_DrawLine(x, y, x, y + h - 1, color);
}

/***************************************************************************************
** Function name:           drawFastHLine
** Description:             draw a horizontal line
***************************************************************************************/
void ILI9341_DrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  // Rudimentary clipping
  if ((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
  if ((x + w - 1) >= ILI9341_WIDTH)  w = ILI9341_WIDTH - x;

  ILI9341_DrawLine(x, y, x + w - 1, y, color);
}

/*----------------------------------------------------------
                Static functions
------------------------------------------------------------*/


static void ILI9341_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) 
{
    // clipping
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
    if((x + w - 1) >= ILI9341_WIDTH) w = ILI9341_WIDTH - x;
    if((y + h - 1) >= ILI9341_HEIGHT) h = ILI9341_HEIGHT - y;

    ILI9341_Select();
    ILI9341_SetAddressWindow(x, y, x+w-1, y+h-1);

    uint8_t data[] = { color >> 8, color & 0xFF };
    GpioPortA_activatePin(ILI9341_DC_Pin);
    for(y = h; y > 0; y--) 
    {
        for(x = w; x > 0; x--) 
        {
          ILI9341_WriteData(data,sizeof(data));
        }
    }

    ILI9341_Unselect();
}

static void ILI9341_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) 
{
    // column address set
    ILI9341_WriteCommand(0x2A); // CASET
    {
        uint8_t data[] = { (x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1 & 0xFF };
        ILI9341_WriteData(data, sizeof(data));
    }

    // row address set
    ILI9341_WriteCommand(0x2B); // RASET
    {
        uint8_t data[] = { (y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1 & 0xFF };
        ILI9341_WriteData(data, sizeof(data));
    }

    // write to RAM
    ILI9341_WriteCommand(0x2C); // RAMWR
}



static void ILI9341_Select(void) 
{
    HalSpi_changeFrequency(SCREEN_FREQ);
    GpioPortA_deactivatePin(ILI9341_CS_Pin);
}

static void ILI9341_Unselect() 
{
    GpioPortA_activatePin(ILI9341_CS_Pin);
}

static void ILI9341_Reset() 
{
    GpioPortA_deactivatePin(ILI9341_RES_Pin);
    Delay_ms(5);
    GpioPortA_activatePin(ILI9341_RES_Pin);
}

static void ILI9341_WriteCommand(uint8_t cmd) 
{
    GpioPortA_deactivatePin(ILI9341_DC_Pin);
    HalSpi_write(cmd);
    //delayMicros(1);
}

static void ILI9341_WriteData(uint8_t* buff, uint32_t buffSize) 
{
    // Configurar el pin DC para indicar que se están enviando datos
    GpioPortA_activatePin(ILI9341_DC_Pin);
    //delayMicros(1);
    // Dividir los datos en bytes y enviar uno por uno
    for (uint32_t i = 0; i < buffSize; ++i) 
    {
        // Llamar a la función de SPI para escribir un byte
        HalSpi_write(buff[i]);
    }
}

static void ILI9341_ReadData(void) 
{
    // para leer datos el pin debe estar activado
    GpioPortA_activatePin(ILI9341_DC_Pin);

    // Dividir los datos en bytes y enviar uno por uno
        // Llamar a la función de SPI para escribir un byte

    (void)HalSpi_read();  /*To be read something in the future*/
}

