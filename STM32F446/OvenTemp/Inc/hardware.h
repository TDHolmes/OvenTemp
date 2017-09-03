/*!
 * @file    hardware.h
 * @author  Tyler Holmes
 * @date    2-Sept-2017
 * @brief   Hardware specific initialization and such.
 */

#pragma once

void SystemClock_Config(void);
void hw_GPIO_Init(void);
void hw_DMA_Init(void);
void hw_ADC1_Init(void);
void hw_I2C1_Init(void);
// void hw_TIM1_Init(void);
void hw_IWDG_Init(void);
void hw_NVIC_Init(void);

ret_t wdg_pet(void);
