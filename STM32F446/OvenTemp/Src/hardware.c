/*!
 * @file    hardware.c
 * @author  Tyler Holmes
 * @date    2-Sept-2017
 * @brief   Hardware specific initialization and such.
 */
#include <stdbool.h>

#include "hardware.h"
#include "common.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_iwdg.h"
#include "stm32f4xx_hal_rtc.h"
#include "stm32f4xx_hal_rtc_ex.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern I2C_HandleTypeDef hI2C3;
extern RTC_HandleTypeDef hrtc;
extern UART_HandleTypeDef huart4;

#define WDG_COUNT  (410u)  // TODO: recalculate

// Private functions

/** System Clock Configuration
*/
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

        /**Configure the main internal regulator output voltage
        */
    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

        /**Initializes the CPU, AHB and APB busses clocks
        */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = 16;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

        /**Initializes the CPU, AHB and APB busses clocks
        */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }

        /**Configure the Systick interrupt time
        */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

        /**Configure the Systick
        */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/** NVIC Configuration
*/
void hw_NVIC_Init(void)
{
    /* ADC_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(ADC_IRQn, 2, 2);  // preempt, sub-priority
    HAL_NVIC_EnableIRQ(ADC_IRQn);
    /* I2C3_EV_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(I2C3_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C3_EV_IRQn);
    /* I2C3_ER_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(I2C3_ER_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C3_ER_IRQn);
    /* DMA2_Stream0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    // TODO: RTC interrupts?
}

/* ADC1 init function */
void hw_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig;

      /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
      */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = ENABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = ENABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfDiscConversion = 1;
    hadc1.Init.NbrOfConversion = NUM_ADC_CHANNELS;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

      /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
      */
    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_112CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

      /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
      */
    sConfig.Channel = ADC_CHANNEL_6;
    sConfig.Rank = 2;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }
}

/* I2C3 init function */
void hw_I2C3_Init(void)
{
    hI2C3.Instance = I2C3;
    hI2C3.Init.ClockSpeed = 100000;
    hI2C3.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hI2C3.Init.OwnAddress1 = 0;
    hI2C3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hI2C3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hI2C3.Init.OwnAddress2 = 0;
    hI2C3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hI2C3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hI2C3) != HAL_OK) {
        Error_Handler();
    }
}

/* RTC init function */
void hw_RTC_Init(void)
{
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    HAL_StatusTypeDef ret;

    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    ret = HAL_RTC_Init(&hrtc);
    if (ret != HAL_OK) {
        Error_Handler_withRetval(ret);
    }

    /* Initialize RTC and set the Time and Date */
    // TODO: Figure out magic value garbage
    if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != 0x32F2) {
        sTime.Hours = 0x0;
        sTime.Minutes = 0x0;
        sTime.Seconds = 0x0;
        sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        sTime.StoreOperation = RTC_STOREOPERATION_RESET;

        ret = HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
        if (ret != HAL_OK) {
            Error_Handler_withRetval(ret);
        }

        sDate.WeekDay = RTC_WEEKDAY_MONDAY;
        sDate.Month = RTC_MONTH_JANUARY;
        sDate.Date = 0x1;
        sDate.Year = 0x0;

        ret = HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
        if (ret != HAL_OK) {
            Error_Handler_withRetval(ret);
        }

        HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0,0x32F2);
    }
}


void hw_RTC_setWakeup(uint32_t timeToWake_ms)
{
    HAL_StatusTypeDef ret;
    // 32,768 ticks/sec (2^15) / 16 (our divider) = 2048 ticks/sec
    ret = HAL_RTCEx_SetWakeUpTimer(&hrtc, timeToWake_ms * 2, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    if (ret != HAL_OK) {
        Error_Handler_withRetval(ret);
    }
}


/* UART4 init function */
void hw_UART4_Init(void)
{
    huart4.Instance = UART4;
    huart4.Init.BaudRate = 115200;
    huart4.Init.WordLength = UART_WORDLENGTH_8B;
    huart4.Init.StopBits = UART_STOPBITS_1;
    huart4.Init.Parity = UART_PARITY_NONE;
    huart4.Init.Mode = UART_MODE_TX_RX;
    huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart4.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart4) != HAL_OK) {
        _Error_Handler(__FILE__, __LINE__);
    }
}

/**
  * Enable DMA controller clock
  */
void hw_DMA_Init(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA2_Stream0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}

/** Pinout Configuration
*/
void hw_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // PA5 is LD2 LED, PA7 and PA2 are timing pins
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    // GPIO_InitStruct.Alternate = 0;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /**ADC1 GPIO Configuration
    PA4   ------> ADC1_IN4
    PA6   ------> ADC1_IN6
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PC13/PC14 are RTC input pins
    GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    // GPIO_InitStruct.Alternate = 0;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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


void hw_LED_setValue(uint8_t value)
{
    if (value == 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    }
}

void hw_LED_toggle(void)
{
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

void hw_TimingPin_setValue(timing_pin_t pin, uint8_t value)
{
    if (value == 0) {
        HAL_GPIO_WritePin(GPIOA, pin, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(GPIOA, pin, GPIO_PIN_SET);
    }
}

void hw_TimingPin_toggle(timing_pin_t pin)
{
    HAL_GPIO_TogglePin(GPIOA, pin);
}
