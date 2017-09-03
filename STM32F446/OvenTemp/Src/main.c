/*!
 * @file    main.c
 * @author  Tyler Holmes
 * @date    2-Sept-2017
 * @brief   Main project logic.

 Copyright at bottom
 */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "common.h"
#include "hardware.h"
#include "stm32f4xx_hal.h"

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;

IWDG_HandleTypeDef hiwdg;

/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
bool wdg_isSet(void);
void wdg_clearFlags(void);


int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    hw_GPIO_Init();
    hw_DMA_Init();
    hw_ADC1_Init();
    hw_I2C1_Init();
    // hw_TIM1_Init();
    hw_IWDG_Init();

    if ( wdg_isSet() ) {
        printf("Watchdog!\n");
    }

    /* Initialize interrupts */
    hw_NVIC_Init();

    /* Infinite loop */
    while (1) {
        // Main loop
        wdg_pet();
    }
}



/*! Checks if the watchdog was the reason for our reset
 *
 * @return true if watchdog was the reason for the reset
 */
bool wdg_isSet(void)
{
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) == RESET) {
        wdg_clearFlags();
        return false;
    } else {
        wdg_clearFlags();
        return true;
    }
}

/*! Private method to reset the reset flags on boot
 */
void wdg_clearFlags(void)
{
    // Clear reset flags
    __HAL_RCC_CLEAR_RESET_FLAGS();
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
    /* User can add his own implementation to report the HAL error return state */
    printf("Error in file `%s`, line %i\n", file, line);
    while(1);
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
    /* User can add his own implementation to report the file name and line number,
      ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}

#endif

/**
  * @}
  */

/**
  * @}
*/

/** This notice applies to any and all portions of this file
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
 ******************************************************************************/
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
