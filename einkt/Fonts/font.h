#include "pico/stdlib.h"
#include "stdio.h"
#include "stdlib.h"

#ifndef __font_H
#define __font_H
typedef struct _tFont
{    
  const char** char_addr;
  const char* width;
  const char* lookup;
  const uint8_t byte_count;
  const uint8_t lenght;
} font;

#endif