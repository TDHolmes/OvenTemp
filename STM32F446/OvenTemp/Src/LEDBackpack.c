/*!
 * @file    LEDBackpack.c
 * @author  Tyler Holmes
 * @date    2-Sept-2017
 * @brief   C library adapted from the quad I2C LED Backpack adafruit code. More info at bottom.
 */

#include <stdint.h>
#include "LEDBackpack.h"
#include "common.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"


static const uint16_t alphafonttable[] = {
    0b0000110000111111, // 0
    0b0000000000000110, // 1
    0b0000000011011011, // 2
    0b0000000010001111, // 3
    0b0000000011100110, // 4
    0b0010000001101001, // 5
    0b0000000011111101, // 6
    0b0000000000000111, // 7
    0b0000000011111111, // 8
    0b0000000011101111, // 9
};

static const uint16_t alpha_point_mask = 0b1000000000000000;

static uint8_t i2c_addr;
static uint16_t displaybuffer[4];

extern I2C_HandleTypeDef hi2c1;

void disp_init(uint8_t addr)
{
    i2c_addr = addr;

    // turn on oscillator
    uint8_t data = 0x21;
    HAL_StatusTypeDef retval = HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)i2c_addr, &data, 1, 5);
    if (retval != HAL_OK) {
        Error_Handler();
    }
    disp_blinkRate(HT16K33_BLINK_OFF);

    disp_setBrightness(15); // max brightness
}

void disp_setBrightness(uint8_t b)
{
    if (b > 15) {
        b = 15;
    }
    uint8_t data = HT16K33_CMD_BRIGHTNESS | b;
    HAL_StatusTypeDef retval = HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)i2c_addr, &data, 1, 5);
    if (retval != HAL_OK) {
        Error_Handler();
    }
}

void disp_blinkRate(uint8_t b)
{
    if (b > 3) {
        b = 0; // turn off if not sure
    }

    uint8_t data = HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | (b << 1);
    HAL_StatusTypeDef retval = HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)i2c_addr, &data, 1, 5);
    if (retval != HAL_OK) {
        Error_Handler();
    }
}

void disp_clear(void) {
    for (uint8_t i = 0; i < 4; i++) {
        displaybuffer[i] = 0;
    }
}

void disp_writeDigit_raw(uint8_t n, uint16_t bitmask) {
    displaybuffer[n] = bitmask;
}

void disp_writeDigit_value(uint8_t n, uint8_t number, bool point) {
    displaybuffer[n] = alphafonttable[number];
    if (point) {
        displaybuffer[n] |= alpha_point_mask;
    }
}

void disp_writeDisplay(void)
{
    // Build up the payload. Start with 0
    uint8_t data[9];
    data[0] = 0;
    for (uint8_t i = 0; i < 4; i++) {
        data[i + 1] = displaybuffer[i] & 0xFF;
        data[i + 2] = displaybuffer[i] >> 8;
    }
    HAL_StatusTypeDef retval = HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)i2c_addr,
                                                       data, 8, 5);
    if (retval != HAL_OK) {
        Error_Handler();
    }
}

/***************************************************
    This is a library for our I2C LED Backpacks

    Designed specifically to work with the Adafruit LED Matrix backpacks
    ----> http://www.adafruit.com/products/
    ----> http://www.adafruit.com/products/

    These displays use I2C to communicate, 2 pins are required to
    interface. There are multiple selectable I2C addresses. For backpacks
    with 2 Address Select pins: 0x70, 0x71, 0x72 or 0x73. For backpacks
    with 3 Address Select pins: 0x70 thru 0x77

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    Written by Limor Fried/Ladyada for Adafruit Industries.
    MIT license, all text above must be included in any redistribution
 ****************************************************/
