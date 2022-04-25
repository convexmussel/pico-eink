#include "DS1307.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
static inline int bcd_decimal(uint8_t hex)
{
    assert(((hex & 0xF0) >> 4) < 10);  // More significant nybble is valid
    assert((hex & 0x0F) < 10);         // Less significant nybble is valid
    int dec = ((hex & 0xF0) >> 4) * 10 + (hex & 0x0F);
    return dec;
}       

void setData()
{
    int return1;
    const uint8_t boe[8] = {0, 0x00, 0x10, 0x61, 0x01, 0x27, 0x09, 0x21};
    return1 = i2c_write_blocking(i2c_default, 104, boe, 8, 0);

    printf("%d", return1);
}

void updateDS(struct timeData *data)
{
    printf("sending commands");
    uint8_t buffer[7];
    uint8_t boe = 0;
    i2c_write_blocking(i2c_default, 104, &boe, 1, 1);
    i2c_read_blocking(i2c_default, 104, buffer, 7, 0);
    data->seconds = bcd_decimal(buffer[0]);
    data->minutes = bcd_decimal(buffer[1]);
    data->hours = bcd_decimal(buffer[2]);
    data->day = bcd_decimal(buffer[3]);
    data->days = bcd_decimal(buffer[4]);
    data->months = bcd_decimal(buffer[5]);
    data->years = bcd_decimal(buffer[6]);
}
bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}
void initDS()
{
    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
 
}