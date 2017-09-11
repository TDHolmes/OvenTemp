/*!
 * @file    LEDBackpack.h
 * @author  Tyler Holmes
 * @date    2-Sept-2017
 * @brief   C library adapted from the I2C LED Backpack adafruit code. More info at bottom.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define DISP_I2C_ADDR  (0x70 << 1)

#define LED_ON 1
#define LED_OFF 0

#define LED_RED 1
#define LED_YELLOW 2
#define LED_GREEN 3

#define HT16K33_BLINK_CMD 0x80
#define HT16K33_BLINK_DISPLAYON 0x01
#define HT16K33_BLINK_OFF 0
#define HT16K33_BLINK_2HZ  1
#define HT16K33_BLINK_1HZ  2
#define HT16K33_BLINK_HALFHZ  3

#define HT16K33_CMD_BRIGHTNESS 0xE0
#define SEVENSEG_DIGITS 5

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
#define BYTE 0

void disp_init(uint8_t addr);
void disp_setBrightness(uint8_t b);
void disp_blinkRate(uint8_t b);
void disp_clear(void);
void disp_writeDigit_raw(uint8_t n, uint16_t bitmask);
void disp_writeDigit_value(uint8_t n, uint8_t number, bool point);
void disp_writeDisplay(void);


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
