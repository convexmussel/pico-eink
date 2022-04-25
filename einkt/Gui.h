#include "pico/stdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "Fonts/font.h"

#ifndef __GUI_H
#define __GUI_H
void initializeDisplay(uint16_t heigth, uint16_t width);
void drawString(const char* text, uint16_t x, uint16_t y, const font* font, bool direction);
void drawChar(const char *character, uint16_t x, uint16_t y, const font* font, uint8_t direction, uint8_t color);
void setPixel(uint8_t color, uint16_t x, uint16_t y);
void paintSolid(uint8_t color);
void show();


#endif