#ifndef __IMAGE_H
#define __IMAGE_H
#include "pico/stdlib.h"
#include "stdio.h"
#include "stdlib.h"
typedef struct Display
{
    uint8_t *image;
    uint16_t height;
    uint16_t width;
}Display;

enum colors{black = 0, white = 1};
enum direction{vertical, horizontal};
#endif