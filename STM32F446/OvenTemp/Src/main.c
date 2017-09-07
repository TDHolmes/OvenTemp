/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
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
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "common.h"
#include "hardware.h"
#include "LEDBackpack.h"
#include "thermocouple.h"
#include "stm32f4xx_hal.h"


#define IDLE_MODE_SLEEPTIME    (60)
#define ACTIVE_MODE_SLEEPTIME  (1)

#define ACTIVE_TEMP_THRESHOLD   (120)  // farenheight

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart4;


typedef enum {
    kIdleMode,
    kActiveMode
} e_main_modes;

e_main_modes mode = kIdleMode;
uint16_t sleep_time;

/* Private function prototypes -----------------------------------------------*/


int main(void)
{
    float temperature;

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    hw_GPIO_Init();
    GPIO_InitTypeDef gpiob_def;
    gpiob_def.Pin = GPIO_PIN_9;
    gpiob_def.Mode = GPIO_MODE_OUTPUT_OD;
    gpiob_def.Pull = GPIO_PULLUP;
    gpiob_def.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &gpiob_def);
    while (1) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
        HAL_Delay(1);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
        HAL_Delay(1);
    }
    hw_DMA_Init();
    hw_ADC1_Init();
    hw_I2C1_Init();

    hw_RTC_Init();
    hw_UART4_Init();

    /* Initialize interrupts */
    hw_NVIC_Init();

    disp_init(DISP_I2C_ADDR);

    // Write out some  test values
    disp_writeDigit_value(0, 1, false);
    disp_writeDigit_value(1, 2, false);
    disp_writeDigit_value(2, 3, false);
    disp_writeDigit_value(3, 4, false);
    disp_writeDisplay();

    therm_init();
    therm_ADC_start(false);  // trigger a single reading...
    /* Infinite loop */
    while (1) {
        // Main loop
        switch (mode) {
            case kIdleMode:
                if ( therm_ADCRunning() ) {
                    if ( therm_valueReady() ) {
                        temperature = therm_getValue_single();
                        if (temperature >= ACTIVE_TEMP_THRESHOLD) {
                            mode = kActiveMode;
                            therm_ADC_start(false);
                        } else {
                            // temperature below needed value. deep sleep for a minute
                            // RTC Clock: 32768 Hz / 16
                            if ( HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, IDLE_MODE_SLEEPTIME,
                                RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK) {
                                Error_Handler();
                            }
                            HAL_PWR_EnterSTANDBYMode();
                        }
                    } else {
                        // Not done yet... Keep snoozin!
                        HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);
                    }
                } else {
                    // Conversion not currently running. Start one and go to sleep
                    // We will get automatically woken up by ADC / DMA interrupt
                    therm_ADC_start(true);
                    HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);
                }
                break;

            case kActiveMode:

                sleep_time = ACTIVE_MODE_SLEEPTIME;
                break;
        }
    }
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
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */

/**
  * @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
