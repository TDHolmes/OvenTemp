/*!
 * @file    stm32f4xx_hal_msp.c
 * @author  Tyler Holmes & STMicroelectronics
 * @date    2-Sept-2017
 * @brief   This file provides code for the MSP Initialization and
                de-Initialization codes.

    Copyright notice at bottom.
 */

#include "stm32f4xx_hal.h"

// Extern variables and functions
extern DMA_HandleTypeDef hdma_adc1;
extern void _Error_Handler(char *, int);


/*! Initializes the Global MSP.
 */
void HAL_MspInit(void)
{
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* System interrupt init*/
    /* MemoryManagement_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
    /* BusFault_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
    /* UsageFault_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
    /* SVCall_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
    /* DebugMonitor_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
    /* PendSV_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/******** ADC FUNCTIONS ***********/

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if(hadc->Instance == ADC1)
    {
        /* Peripheral clock enable */
        __HAL_RCC_ADC1_CLK_ENABLE();

        /**ADC1 GPIO Configuration
        PA4   ------> ADC1_IN4
        PA6   ------> ADC1_IN6
        */
        GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* ADC1 interrupt Init */
        HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(ADC_IRQn);
    }
}


void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{

    if(hadc->Instance == ADC1)
    {

        /* Peripheral clock disable */
        __HAL_RCC_ADC1_CLK_DISABLE();

        /**ADC1 GPIO Configuration
        PA4  ------> ADC1_IN4
        PA6  ------> ADC1_IN6
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4 | GPIO_PIN_6);

        /* ADC1 interrupt DeInit */
        HAL_NVIC_DisableIRQ(ADC_IRQn);
    }
}


/******** I2C FUNCTIONS ***********/


void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if(hi2c->Instance == I2C3)
    {

        /**I2C3 GPIO Configuration
        PA8   ------> I2C3_SCL
        PC9   ------> I2C3_SDA
        */
        GPIO_InitStruct.Pin = GPIO_PIN_8;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        // Now SDA
        GPIO_InitStruct.Pin = GPIO_PIN_9;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        /* Peripheral clock enable */
        __HAL_RCC_I2C3_CLK_ENABLE();
    }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
    if(hi2c->Instance == I2C3)
    {
        /* Peripheral clock disable */
        __HAL_RCC_I2C3_CLK_DISABLE();

        /**I2C3 GPIO Configuration
        PA8  ------> I2C3_SCL
        PC9  ------> I2C3_SDA
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8);
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_9);

        /* I2C3 interrupt DeInit */
        HAL_NVIC_DisableIRQ(I2C3_EV_IRQn);
        HAL_NVIC_DisableIRQ(I2C3_ER_IRQn);
    }
}


/******** RTC FUNCTIONS ***********/


void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
    if(hrtc->Instance == RTC)
    {
        /* Peripheral clock enable */
        __HAL_RCC_RTC_ENABLE();
    }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{
    if(hrtc->Instance == RTC)
    {
        /* Peripheral clock disable */
        __HAL_RCC_RTC_DISABLE();
    }
}


/******** UART FUNCTIONS ***********/


void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if(huart->Instance == UART4)
    {
        /* Peripheral clock enable */
        __HAL_RCC_UART4_CLK_ENABLE();

        /**UART4 GPIO Configuration
        PA0-WKUP ------> UART4_TX
        PA1      ------> UART4_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
    if(huart->Instance == UART4)
    {
        /* Peripheral clock disable */
        __HAL_RCC_UART4_CLK_DISABLE();

        /**UART4 GPIO Configuration
        PA0-WKUP ------> UART4_TX
        PA1      ------> UART4_RX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1);
    }
}


/**
  * @}
  */

/**
  * @}
  */

/*******************************************************************************
* This notice applies to any and all portions of this file
* that are not made or edited by Tyler Holmes.
* Other portions of this file, whether
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
************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
