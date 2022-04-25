#ifndef bme280_H
#define bme280_H

#include "pico/stdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "sensorData.h"

/**
 * GPIOI config
**/

float compensate_temp(uint32_t temp_adc);
float compensate_pressure(uint32_t adc_P);
float compensate_humidity(int32_t adc_H);
static void read_registers(uint8_t reg, uint8_t *buf, uint16_t len);
void read_compensation_parameters();
static void bme280_read_raw(uint32_t *humidity, uint32_t *pressure, uint32_t *temperature, uint32_t *gas);
int setupBME();
void updateBME(struct sensorData *data);
static bool setGasWaitTime(uint8_t time, uint8_t multiplication, uint8_t heater);
static bool setTargetHeaterResitance(uint8_t heater, uint16_t targetTemp, uint8_t ambientTemp);
static bool setRunGas(bool enable);
static bool setHeaterSetProfile(uint8_t heater);
static bool enableHeaterCurrent();
#endif