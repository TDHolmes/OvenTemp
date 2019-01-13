/*!
 * @file    stm32f4xx_it.c
 * @author  Tyler Holmes & STMicroelectronics
 * @date    2-Sept-2017
 * @brief   Location where all system & peripheral interrupts are serviced.

    Copyright notice at bottom.
 */

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "thermocouple.h"
#include "hardware.h"


// External variables
extern DMA_HandleTypeDef hdma_adc1;
extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hI2C3;
extern RTC_HandleTypeDef hrtc;

/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
    while (1);  // We shouldn't hit an NMI AFAIK.
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
    while (1);
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
    while (1);
}

/**
* @brief This function handles Pre-fetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
    while (1);
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
    while (1);
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
    while (1);  // Non critical error, but still catch it for debugging.
}

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
    while (1);  // Non critical error, but still catch it for debugging.
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
    while (1);  // Non critical error, but still catch it for debugging.
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief  This function handles RTC Auto wake-up interrupt request.
  * @param  None
  * @retval None
  */
void RTC_WKUP_IRQHandler(void)
{
    HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);
}


/**
* @brief This function handles I2C3 event interrupt.
*/
void I2C3_EV_IRQHandler(void)
{
    // hw_TimingPin_setValue(kTimingPin_D11, 1);
    HAL_I2C_EV_IRQHandler(&hI2C3);
    // hw_TimingPin_setValue(kTimingPin_D11, 0);
}

/**
* @brief This function handles I2C3 error interrupt.
*/
void I2C3_ER_IRQHandler(void)
{
    hw_TimingPin_setValue(kTimingPin_D11, 1);
    HAL_I2C_ER_IRQHandler(&hI2C3);
    hw_TimingPin_setValue(kTimingPin_D11, 0);
}

/**
* @brief This function handles ADC1, ADC2 and ADC3 interrupts.
*/
void ADC_IRQHandler(void)
{
    // hw_TimingPin_setValue(kTimingPin_D11, 1);
    HAL_ADC_IRQHandler(&hadc1);
    // hw_TimingPin_setValue(kTimingPin_D11, 0);
}

/**
* @brief This function handles DMA2 stream0 global interrupt.
*/
void DMA2_Stream0_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_adc1);
}

/*******************************************************************************
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
************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
