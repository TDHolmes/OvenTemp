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

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

extern I2C_HandleTypeDef hi2c1;

extern TIM_HandleTypeDef htim1;

extern IWDG_HandleTypeDef hiwdg;

#define WDG_COUNT  (410u)  // TODO: recalculate

// Private functions
static void hw_wdg_clearFlags(void);


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
    HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);  // preempt, sub-priority
    HAL_NVIC_EnableIRQ(ADC_IRQn);
    /* I2C1_EV_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
    /* I2C1_ER_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
    /* DMA2_Stream0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}

/* ADC1 init function */
void hw_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig;

    /** Configure the global features of the ADC (Clock, Resolution, Data
        Alignment and number of conversion)  */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in
        the sequencer and its sample time. */
    // Pin A4  (TODO: Vout from thermocouple?)
    sConfig.Channel = ADC_CHANNEL_5;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    // Pin A5  (TODO: Vref from thermocouple?)
    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.Rank = 2;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
    // TODO: configure GPIO pins A4 and A5 for analog?
}

/* I2C1 init function */
void hw_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        Error_Handler();
    }
}


/* TIM1 init function */ /*
void hw_TIM1_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 0;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 0;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
    {
        Error_Handler();
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }
}  */

/**
  * Enable DMA controller clock
  */
void hw_DMA_Init(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();
}

/** Pinout Configuration
*/
void hw_GPIO_Init(void)
{
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
}


/*! Initializes the independent watchdog module to require a pet every N ms  TODO: update timing
 *
 * @return success or failure of initializing the watchdog module
 */
void hw_IWDG_Init(void)
{
    // the LSI counter used for wdg timer is @41KHz.  # TODO: update math
    // 10 ms counter window count: counter_val = (10 ms) * (41 kHz) ~= 410
    hiwdg.Instance = IWDG;
    //! Select the prescaler of the IWDG. This parameter can be a value of @ref IWDG_Prescaler
    hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
    /*! Specifies the IWDG down-counter reload value. This parameter must
      be a number between Min_Data = 0 and Max_Data = 0x0FFFU */
    hiwdg.Init.Reload = WDG_COUNT;

    if (HAL_IWDG_Init(&hiwdg) != HAL_OK) {
        Error_Handler();
    }
    __HAL_DBGMCU_FREEZE_IWDG();
}

/*! Resets the watchdog timer (pets it) so we don't reset
 *
 * @return success or failure of petting the watchdog
 */
ret_t hw_wdg_pet(void)
{
    if (HAL_IWDG_Refresh(&hiwdg) == HAL_OK) {
        return RET_OK;
    } else {
        return RET_GEN_ERR;
    }
}

/*! Checks if the watchdog was the reason for our reset
 *
 * @return true if watchdog was the reason for the reset
 */
bool hw_wdg_isSet(void)
{
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) == RESET) {
        hw_wdg_clearFlags();
        return false;
    } else {
        hw_wdg_clearFlags();
        return true;
    }
}

/*! Private method to reset the reset flags on boot
 */
static void hw_wdg_clearFlags(void)
{
    // Clear reset flags
    __HAL_RCC_CLEAR_RESET_FLAGS();
}
