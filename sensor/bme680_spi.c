/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "bme680_spi.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

/* Example code to talk to a bme280 humidity/temperature/pressure sensor.

   NOTE: Ensure the device is capable of being driven at 3.3v NOT 5v. The Pico
   GPIO (and therefor SPI) cannot be used at 5v.

   You will need to use a level shifter on the SPI lines if you want to run the
   board at 5v.

   Connections on Raspberry Pi Pico board and a generic bme280 board, other
   boards may vary.

   GPIO 16 (pin 21) MISO/spi0_rx-> SDO/SDO on bme280 board
   GPIO 17 (pin 22) Chip select -> CSB/!CS on bme280 board
   GPIO 18 (pin 24) SCK/spi0_sclk -> SCL/SCK on bme280 board
   GPIO 19 (pin 25) MOSI/spi0_tx -> SDA/SDI on bme280 board
   3.3v (pin 36) -> VCC on bme280 board
   GND (pin 38)  -> GND on bme280 board

   Note: SPI devices can have a number of different naming schemes for pins. See
   the Wikipedia page at https://en.wikipedia.org/wiki/Serial_Peripheral_Interface
   for variations.

   This code uses a bunch of register definitions, and some compensation code derived
   from the Bosch datasheet which can be found here.
   https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf
*/

#define GAS_WAIT_0 0x64
#define HEATER_SETPOINT_0 0x5A
#define ADRESS 0x77
#define READ_BIT 0x80
#define CTRL_GAS_1 0x71

#define RUN_GAS_BIT 4

uint8_t reg = 0;
float T;
float calc_temp;

uint16_t dig_T1;
int16_t dig_T2;
int8_t dig_T3;
uint16_t dig_P1;
int16_t dig_P2;
int8_t dig_P3;
int16_t dig_P4;
int16_t dig_P5;
int8_t dig_P6;
int8_t dig_P7;
int16_t dig_P8;
int16_t dig_P9;
uint8_t dig_P10;

int16_t dig_H1, dig_H2;
uint8_t dig_H4, dig_H5, dig_H7, dig_H3, dig_H6;

int8_t dig_G1, dig_G3;
int16_t dig_G2;
int8_t res_heat_range;
int8_t res_heat_val;
int8_t range_switch_error;

int8_t gas_range;

/* The following compensation functions are required to convert from the raw ADC
data from the chip to something usable. Each chip has a different set of
compensation parameters stored on the chip at point of manufacture, which are
read from the chip at startup and used inthese routines.
*/

static float calc_res_heat(uint8_t ambient_temp, uint16_t target_temp){
    float var1;
    float var2;
    float var3;
    float var4;
    float var5;
    uint8_t res_heat;

    if (target_temp > 400) /* Cap temperature */
    {
        target_temp = 400;
    }

    var1 = (((float)dig_G1 / (16.0f)) + 49.0f);
    var2 = ((((float)dig_G2 / (32768.0f)) * (0.0005f)) + 0.00235f);
    var3 = ((float)dig_G3 / (1024.0f));
    var4 = (var1 * (1.0f + (var2 * (float)target_temp)));
    var5 = (var4 + (var3 * (float)ambient_temp));
    res_heat =
        (uint8_t)(3.4f *
                  ((var5 * (4 / (4 + (float)res_heat_range)) *
                    (1 / (1 + ((float)res_heat_val * 0.002f)))) -
                   25));

    return res_heat;
}

/* This internal API is used to calculate the gas resistance low value in float */
static float calc_gas_resistance_low(uint16_t gas_res_adc)
{
    float calc_gas_res;
    float var1;
    float var2;
    float var3;
    float gas_res_f = gas_res_adc;
    float gas_range_f = (1U << gas_range); /*lint !e790 / Suspicious truncation, integral to float */
    const float lookup_k1_range[16] = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, -0.8f, 0.0f, 0.0f, -0.2f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f
    };
    const float lookup_k2_range[16] = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.7f, 0.0f, -0.8f, -0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
    };

    var1 = (1340.0f + (5.0f * range_switch_error));
    var2 = (var1) * (1.0f + lookup_k1_range[gas_range] / 100.0f);
    var3 = 1.0f + (lookup_k2_range[gas_range] / 100.0f);
    calc_gas_res = 1.0f / (float)(var3 * (0.000000125f) * gas_range_f * (((gas_res_f - 512.0f) / var2) + 1.0f));

    return calc_gas_res;
}
/* @brief This internal API is used to calculate the temperature value. */
float compensate_temp(uint32_t temp_adc)
{
    float var1;
    float var2;
    float calc_temp;
    float t_fine;


    /* calculate var1 data */
    var1 = ((((float)temp_adc / 16384.0f) - ((float)dig_T1 / 1024.0f)) * ((float)dig_T2));

    /* calculate var2 data */
    var2 =
        (((((float)temp_adc / 131072.0f) - ((float)dig_T1 / 8192.0f)) *
          (((float)temp_adc / 131072.0f) - ((float)dig_T1 / 8192.0f))) * ((float)dig_T3 * 16.0f));

    /* t_fine value*/
    t_fine = (var1 + var2);
    T = t_fine;
    /* compensated temperature data*/
    calc_temp = ((t_fine) / 5120.0f);

    return calc_temp;
}

float compensate_pressure(uint32_t adc_P) {
   float var1;
    float var2;
    float var3;
    float calc_pres;

    var1 = (((float)T / 2.0f) - 64000.0f);
    var2 = var1 * var1 * (((float)dig_P6) / (131072.0f));
    var2 = var2 + (var1 * ((float)dig_P5) * 2.0f);
    var2 = (var2 / 4.0f) + (((float)dig_P4) * 65536.0f);
    var1 = (((((float)dig_P3 * var1 * var1) / 16384.0f) + ((float)dig_P2 * var1)) / 524288.0f);
    var1 = ((1.0f + (var1 / 32768.0f)) * ((float)dig_P1));
    calc_pres = (1048576.0f - ((float)adc_P));

    /* Avoid exception caused by division by zero */
    if ((int)var1 != 0)
    {
        calc_pres = (((calc_pres - (var2 / 4096.0f)) * 6250.0f) / var1);
        var1 = (((float)dig_P9) * calc_pres * calc_pres) / 2147483648.0f;
        var2 = calc_pres * (((float)dig_P8) / 32768.0f);
        var3 = ((calc_pres / 256.0f) * (calc_pres / 256.0f) * (calc_pres / 256.0f) * (dig_P10 / 131072.0f));
        calc_pres = (calc_pres + (var1 + var2 + var3 + ((float)dig_P7 * 128.0f)) / 16.0f);
    }
    else
    {
        calc_pres = 0;
    }

    return calc_pres;
}

float compensate_humidity(int32_t adc_H) {
    float calc_hum;
    float var1;
    float var2;
    float var3;
    float var4;
    float temp_comp;

    /* compensated temperature data*/
    temp_comp = ((T) / 5120.0f);
    var1 = (float)((float)adc_H) -
           (((float)dig_H1 * 16.0f) + (((float)dig_H3 / 2.0f) * temp_comp));
    var2 = var1 *
           ((float)(((float)dig_H2 / 262144.0f) *
                    (1.0f + (((float)dig_H4 / 16384.0f) * temp_comp) +
                     (((float)dig_H5 / 1048576.0f) * temp_comp * temp_comp))));
    var3 = (float)dig_H6 / 16384.0f;
    var4 = (float)dig_H7 / 2097152.0f;
    calc_hum = var2 + ((var3 + (var4 * temp_comp)) * var2 * var2);
    if (calc_hum > 100.0f)
    {
        calc_hum = 100.0f;
    }
    else if (calc_hum < 0.0f)
    {
        calc_hum = 0.0f;
    }

    return calc_hum;
}

static inline void cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);
    asm volatile("nop \n nop \n nop");
}

static int write_register(uint8_t reg, uint8_t data) {
    uint8_t buf[2] = {reg, data};
    return i2c_write_blocking(i2c1, ADRESS, buf, 2, false);
}

static void read_registers(uint8_t reg, uint8_t *buf, uint16_t len) {
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.
    i2c_write_blocking(i2c1, ADRESS, &reg, 1, false);
    i2c_read_blocking(i2c1, ADRESS, buf, len, false);
    sleep_ms(10);
}

/* This function reads the manufacturing assigned compensation parameters from the device */
void read_compensation_parameters() {
    uint8_t buffer1[25];
    uint8_t buffer2[16];
    uint8_t buffer_low[5];
    
    read_registers(0x00, buffer_low, 5);
    read_registers(0x89, buffer1, 25);
    read_registers(0xE1, buffer2, 16);

    dig_T1 = ((uint16_t)buffer2[8]) + (((uint16_t)buffer2[9]) << 8);
    dig_T2 = ((int16_t)buffer1[1]) + (((int16_t)buffer1[2]) << 8);
    dig_T3 = buffer1[3];

    dig_P1 = (uint32_t)buffer1[5] + (((uint32_t)buffer1[6]) << 8);
    dig_P2 = (uint32_t)buffer1[7] + ((((uint32_t)buffer1[8])) << 8);
    dig_P3 = (uint32_t)buffer1[9];
    dig_P4 = (uint32_t)buffer1[11] + (((uint32_t)buffer1[12]) << 8);
    dig_P5 = (uint32_t)buffer1[13] + (((uint32_t)buffer1[14]) << 8);
    dig_P6 = (uint32_t)buffer1[16];
    dig_P7 = (uint32_t)buffer1[15];
    dig_P8 = (uint32_t)buffer1[19] + (((uint32_t)buffer1[20]) << 8);
    dig_P9 = (uint32_t)buffer1[21] + (((uint32_t)buffer1[22]) << 8);
    dig_P10 = (uint32_t)buffer1[23];

    dig_H1 = ((uint32_t)buffer2[1] && 0x0F) | ((uint32_t)buffer2[2] << 4);
    dig_H2 = ((uint32_t)buffer2[1]  && 0xF0) | ((uint32_t)buffer2[0] << 4);
    dig_H3 = (uint32_t) buffer2[3];
    dig_H4 = (uint32_t) buffer2[4];
    dig_H5 = (uint32_t) buffer2[5];
    dig_H6 = (uint32_t) buffer2[6];
    dig_H7 = (uint32_t) buffer2[7];

    dig_G1 = buffer2[12];
    dig_G2 =((int16_t) buffer2[10]) |  (((int16_t)buffer2[11]) << 8);
    dig_G3 = buffer2[13];

    res_heat_range = (buffer_low[2] & 0x30) >> 4;
    res_heat_val = buffer_low[0];

}

static void bme280_read_raw(uint32_t *humidity, uint32_t *pressure, uint32_t *temperature, uint32_t *gas) {
    uint8_t buffer[15];
    uint8_t wait;
    uint8_t switch_error;
    uint8_t test; 
    read_registers(0x1D, &wait, 1);
    read_registers(0x1D, &switch_error, 1);
    while(wait & 0b01100000)    
    {   
        read_registers(0x1D, &wait, 1);
    }
    read_registers(0x1D, buffer, 15);

    read_registers(0x2b, &test, 1);
    printf("%d\n", test);
    range_switch_error = (((int8_t)switch_error) & 0xF0) >> 4;
    *temperature = (uint32_t)(((uint32_t)buffer[5] * 4096) | ((uint32_t)buffer[6] * 16) | ((uint32_t)buffer[7] / 16));     
    *pressure = (uint32_t)(((uint32_t)buffer[2] * 4096) | ((uint32_t)buffer[3] * 16) | ((uint32_t)buffer[4] / 16));                
    *humidity = (((uint32_t)buffer[8]) << 8) | buffer[9];
    *gas      = (((uint32_t)buffer[13]) << 2) | buffer[14] >> 6;
}

void updateBME(struct sensorData *data)
{
    uint32_t humidity, pressure, temperature, gas;
    float test, banana, carrot;
    setRunGas(true);
    enableHeaterCurrent();
    setHeaterSetProfile(1);
    setGasWaitTime(37, 1, 1);
    setTargetHeaterResitance(1, 250, 19);
    
    read_registers(0x74, &reg, 1);
    write_register(0x74, reg + 1);
    bme280_read_raw(&humidity, &pressure, &temperature, &gas);

    data->temp = compensate_temp(temperature);
    data->humidity = compensate_humidity(humidity);
    data->pressure = compensate_pressure(pressure);
    data->gas = calc_gas_resistance_low(gas);
}

/**
 *  Set the gas measurement wait time for a specific heater 0-9 
 *  Tme is MS max 64 ms and multiplication follows the following table
 *  0 = 1x
 *  1 = 4x
 *  2 = 16x
 *  3 = 64x
**/
static bool setGasWaitTime(uint8_t time, uint8_t multiplication, uint8_t heater)
{
    //if wait time is to large cut it off
    time &= 0x1f;
    if(write_register(GAS_WAIT_0 + heater, time | (multiplication << 6) ) > 0) 
    {
        return true;
    }
    return false;
}

static bool setTargetHeaterResitance(uint8_t heater, uint16_t targetTemp, uint8_t ambientTemp)
{
    float resistance;
    resistance = calc_res_heat(ambientTemp, targetTemp);

    return write_register(HEATER_SETPOINT_0 + heater, (uint8_t)resistance);
}

static bool setRunGas(bool enable)
{
    uint8_t currentValueRegister;
    uint8_t msk = 0xEF;

     //read current value in register
    read_registers(CTRL_GAS_1, &currentValueRegister, 1);
    
    //remove the to be removed bits
    currentValueRegister &=  msk;
    //set the run gas bit
    currentValueRegister |= (((uint8_t)enable) << RUN_GAS_BIT);
    //write the value into the register
    write_register(CTRL_GAS_1, currentValueRegister);

    return true;
}

static bool setHeaterSetProfile(uint8_t heater)
{
    uint8_t currentValueRegister;
    uint8_t msk = 0xF0;

    //check if imput is valid
    if(heater > 0x0F) return false;
    //read current value in register
    read_registers(CTRL_GAS_1, &currentValueRegister, 1);
    
    //remove the to be removed bits
    currentValueRegister &=  msk;
    //add the heater to the register value
    currentValueRegister |= heater;

    //write the value into the register
    write_register(CTRL_GAS_1, currentValueRegister);

    return true;
}

static bool enableHeaterCurrent(){
    return write_register(0x70, 0); // turn on current
}

static bool disableHeaterCurrent(){
    return write_register(0x70, 1); // turn off current
}

int setupBME() {
    stdio_init_all();
    //printf("Hello, bme280! Reading raw data from registers via SPI...\n");
        // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    i2c_init(i2c1, 100 * 1000);
    gpio_set_function(18, GPIO_FUNC_I2C);
    gpio_set_function(19, GPIO_FUNC_I2C);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(18, 19, GPIO_FUNC_I2C));
   
    
    // See if SPI is working - interrograte the device for its I2C ID number, should be 0x60
    uint8_t id;
    read_registers(0xD0, &id, 1);
    //printf("Chip ID is 0x%x\n", id);
    //read_compensation_parameters();
    write_register(0x72, 0x01); // Humidity oversampling register - going for x1
    write_register(0x74, 0x66);// Set rest of oversampling modes and run mode to normal
    write_register(0x64, 0x59);
    write_register(0x5A, 0x35);
    write_register(0x71, 0x10);
    write_register(0x75, 0x6C);
    write_register(0x70, 0x08); // turn off current
   
    read_compensation_parameters();
    return 0;
}
