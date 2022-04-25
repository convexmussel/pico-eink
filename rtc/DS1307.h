#ifndef DS1307_H
#define DS1307_H

#include "pico/stdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "timeData.h"

/**
 * GPIOI config
**/
extern int EPD_RST_PIN;
extern int EPD_DC_PIN;
extern int EPD_CS_PIN;
extern int EPD_BUSY_PIN;
extern int EPD_CLK_PIN;
extern int EPD_MOSI_PIN;


void initDS();
void updateDS(struct timeData *data);

void DEV_Digital_Write(uint16_t Pin, uint8_t Value);
uint8_t DEV_Digital_Read(uint16_t Pin);

void DEV_SPI_WriteByte(uint8_t Value);
void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len);
void DEV_Delay_ms(uint32_t xms);
void DEV_Module_Exit(void);



#endif
