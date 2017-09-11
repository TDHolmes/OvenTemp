/*!
 * @file    hardware.h
 * @author  Tyler Holmes
 * @date    2-Sept-2017
 * @brief   Hardware specific initialization and such.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "common.h"


void SystemClock_Config(void);
void hw_GPIO_Init(void);
void hw_DMA_Init(void);
void hw_ADC1_Init(void);
void hw_I2C3_Init(void);
void hw_RTC_Init(void);
void hw_UART4_Init(void);
void hw_NVIC_Init(void);


void hw_LED_setValue(uint8_t value);
void hw_LED_toggle(void);
