/*!
 * @file    hardware.c
 * @author  Tyler Holmes
 * @date    2-Sept-2017
 * @brief   Hardware specific initialization and such.
 *
 * @note    Stupid STM copyright notice at end.
  */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "hardware.h"
#include "display.h"
#include "thermocouple.h"
#include "stm32f4xx_hal.h"


//! How long we sleep between readings in idle mode, in seconds.
#define IDLE_MODE_SLEEPTIME    (60)
//! How long we sleep between readings in active mode, in seconds.
#define ACTIVE_MODE_SLEEPTIME  (1)

//! threshold to switch to active mode, in farenheight
#define ACTIVE_TEMP_THRESHOLD   (120)


ADC_HandleTypeDef hadc1;      //!< HAL handle for ADC1
DMA_HandleTypeDef hdma_adc1;  //!< HAL handle for DMA for ADC1. Currently not used.
I2C_HandleTypeDef hI2C3;      //!< HAL handle for I2C3 (display)
RTC_HandleTypeDef hrtc;       //!< HAL handle for our RTC. Used to awake us from deep sleep
UART_HandleTypeDef huart4;    //!< HAL handle for UART4 for debug prints


//! Different modes the main loop can be in based on the oven temperature.
typedef enum {
    kIdleMode,
    kActiveMode
} e_main_modes;

static e_main_modes mode = kIdleMode;  //!< global tracking main's state
uint8_t str_buff[64];  //!< buffer for transmitting data over UART

/****  Private function definitions  ****/
void blocking_delay(volatile uint32_t delay);
void displayTemp(float temp);


/*! Main function. Initializes all peripherals needed, get's an initial thermocouple
 *   reading, then goes into either active or idle mode. We then continue, checking
 *   the battery voltage
 */
int main(void)
{
    float temperature;

    //! Reset of all peripherals, Initializes the Flash interface and the Systick.
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    hw_GPIO_Init();
    hw_ADC1_Init();
    hw_I2C3_Init();
    hw_RTC_Init();
    hw_UART4_Init();

    /* Initialize interrupts */
    hw_NVIC_Init();
    hw_LED_setValue(0);

    // Initialize display and clear all digits
    disp_init(DISP_I2C_ADDR);
    disp_writeDigit_raw(0, 0);
    disp_writeDigit_raw(1, 0);
    disp_writeDigit_raw(2, 0);
    disp_writeDigit_raw(3, 0);
    disp_writeDisplay();

    therm_init();
    therm_startReading_single();  // trigger a single reading...
    // Block on getting an initial reading in order to initialize state properly.
    while (1) {
        if ( therm_valueReady() ) {
            temperature = therm_getValue_single();
            if (temperature >= ACTIVE_TEMP_THRESHOLD) {
                mode = kActiveMode;
                therm_startReading_continuous();
            } else {
                mode = kIdleMode;
            }
            break;
        }
    }


    //  Main infinite loop
    while (1) {
        switch (mode) {
            case kIdleMode:
                if ( therm_valueReady() ) {
                    temperature = therm_getValue_single();
                    if (temperature >= ACTIVE_TEMP_THRESHOLD) {
                        mode = kActiveMode;
                        therm_startReading_continuous();  // Continuous thermocouple conversion
                    } else {
                        // temperature below needed value. deep sleep for a minute
                        HAL_StatusTypeDef ret = HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, IDLE_MODE_SLEEPTIME,
                                                                            RTC_WAKEUPCLOCK_RTCCLK_DIV16);
                        if (ret != HAL_OK) {
                            Error_Handler_withRetval(__FILE__, __LINE__, (int)ret);
                        }
                        HAL_PWR_EnterSTANDBYMode();
                        therm_startReading_single();  // Start a single thermocouple read after we wake back up
                    }
                } else {
                    // Not done yet... Keep snoozin! ADC interrupt should wake us from SLEEP
                    HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);
                }
                break;

            case kActiveMode:
                if ( therm_valueReady() ) {
                    temperature = therm_getValue_averaged();
                    if (temperature < ACTIVE_TEMP_THRESHOLD) {
                        mode = kIdleMode;
                        therm_startReading_single();  // Single thermocouple conversion
                    } else {
                        // temperature at needed value. Display temp
                        displayTemp(temperature);
                        // deep sleep for a second to save power
                        HAL_StatusTypeDef ret = HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, ACTIVE_MODE_SLEEPTIME,
                                                                            RTC_WAKEUPCLOCK_RTCCLK_DIV16);
                        if (ret != HAL_OK) {
                            Error_Handler_withRetval(__FILE__, __LINE__, (int)ret);
                        }
                        HAL_PWR_EnterSTANDBYMode();
                    }
                } else {
                    // Not done yet... Keep snoozin! ADC interrupt should wake us from SLEEP
                    HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);
                }
                break;
        }
    }
}


/*! This function takes in a positive, floating point value from [0, 1000) and
 *   displays it on the four digit display we have with the most percision possible.
 */
void displayTemp(float temp)
{
    uint8_t a,b,c,d = 0;
    if (temp < 100) {
        a = (uint8_t)(((uint32_t)temp % 100) / 10);    // 10's place
        b = (uint8_t)((uint32_t)temp % 10);            // 1's place
        c = (uint8_t)((uint32_t)(temp * 10.0f) % 10);  // 10ths place
        d = (uint8_t)((uint32_t)(temp * 100.0f) % 10); // 100ths place

        // Write the dot out
        disp_writeDigit_value(1, b, true);
        disp_writeDigit_value(2, c, false);
    } else {
        a = (uint8_t)(((uint32_t)temp % 1000) / 100);  // 100's place
        b = (uint8_t)(((uint32_t)temp % 100) / 10);    // 10's place
        c = (uint8_t)((uint32_t)temp % 10);            // 1's place
        d = (uint8_t)((uint32_t)(temp * 10.0f) % 10);  // 10ths place

        // Write the dot out
        disp_writeDigit_value(1, b, false);
        disp_writeDigit_value(2, c, true);
    }

    //write out the rest
    if (temp < 10) {
        disp_writeDigit_raw(0, 0);  // Clear this section
    } else {
        disp_writeDigit_value(0, a, false);
    }
    disp_writeDigit_value(3, d, false);

    disp_writeDisplay();
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    Error_Handler_withRetval(file, line, 0);
}

/*! This function is called when there is a fatal error somewhere in the code.
 *   The current behavior is to blink rapidly and send a formatted string over
 *   UART.
 * @param  file (char *): Name of the file in which the error occurred.
 * @param  line (int): Line number on which the error occurred.
 * @param  retval (int): The error code for the failure.
 */
void Error_Handler_withRetval(char * file, int line, int retval)
{
    sprintf((char *)str_buff, "%s:%i  ->  %i\r\n", file, line, retval);
    while(1)
    {
        hw_LED_setValue(0);
        blocking_delay(300000);
        hw_LED_setValue(1);
        blocking_delay(300000);
        HAL_UART_Transmit(&huart4, str_buff, strlen((const char *)str_buff), 10);
    }
}


/*! Simple, dumb delay function to be used when we can't necessarily rely on
 *   interrupts to do delay timing. Normally, you should use interrupt driven or
 *   HAL_Delay() to rely on the systick.
 */
inline void blocking_delay(volatile uint32_t delay) {
    for (; delay != 0; delay--);
}


/********************** BEGIN STUPID COPYRIGHT NOTICE **************************
** This notice applies to any and all portions of this file
* that are not made by Tyler Holmes. Other portions of this file, whether
* inserted by the user or by software development tools
* are owned by their respective copyright owners.
*
* COPYRIGHT(c) 2017 STMicroelectronics
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/
