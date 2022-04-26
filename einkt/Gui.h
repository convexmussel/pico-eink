#include "pico/stdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "Fonts/gfxfont.h"
#include "EPD_7in5_V2.h"

#ifndef __GUI_H
#define __GUI_H
void initializeDisplay(uint16_t heigth, uint16_t width);
void drawString(const char* text, uint16_t x, uint16_t y, const GFXfont *font, bool direction, uint16_t textRotation);
void drawChar(const char *character, uint16_t x, uint16_t y, const GFXfont *font, bool direction, uint16_t textRotation);
void drawNumber(const uint16_t number,  uint16_t x, uint16_t y, const GFXfont *font, bool direction, uint16_t textRotation);
void setPixel(uint8_t color, uint16_t x, uint16_t y);
void paintSolid(uint8_t color);
void show(bool speed);


#endif