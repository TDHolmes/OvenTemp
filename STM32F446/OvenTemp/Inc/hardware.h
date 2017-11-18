/*!
 * @file    hardware.h
 * @author  Tyler Holmes
 * @date    2-Sept-2017
 * @brief   Hardware specific initialization and such.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "stm32f4xx_hal.h"
#include "common.h"

#define NUM_ADC_CHANNELS  (2)  //!< Thermocouple vout and voltage ref


//! Enum of the timing pins we use.
typedef enum {
    kTimingPin_D11 = GPIO_PIN_7,
    kTimingPin_D01 = GPIO_PIN_2,
} timing_pin_t;


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

void hw_TimingPin_setValue(timing_pin_t pin, uint8_t value);
void hw_TimingPin_toggle(timing_pin_t pin);
