/*!
 * @file    thermocouple.c
 * @author  Tyler Holmes
 * @date    2-Sept-2017
 * @brief   Interface to the thermocouple temperature sensor
 */
// #include <stdbool.h>
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
// #include "stm32f4xx_hal_dma.h"
#include "common.h"

#define NUM_READINGS  (8)  // TODO: justify
#define THERM_GET_IDEX()   (vref_index == NUM_READINGS - 1 ? 0 : vref_index)

extern ADC_HandleTypeDef hadc1;

static float vout_readings[NUM_READINGS];
static float vref_readings[NUM_READINGS];

static uint32_t pending_readings[2];

static uint8_t vout_index = 0;
static uint8_t vref_index = 0;
static bool keep_converting = true;
static bool reading_ready = false;
static bool ADC_running = false;


void therm_init(void)
{
    for (int8_t i = NUM_READINGS - 1; i > 0; i--) {
        vout_readings[i] = 0;
        vref_readings[i] = 0;
    }
    pending_readings[0] = 0;
    pending_readings[1] = 0;
    reading_ready = false;
}

void therm_ADC_start(bool single)
{
    // Start an ADC conversions for both channels
    if (HAL_ADC_Start_DMA(&hadc1, pending_readings, 2) != HAL_OK) {
        Error_Handler();
    }
    if (single) {
        reading_ready = false;
    }
    keep_converting = !single;
    ADC_running = true;
}

void therm_ADC_stop(bool immediate)
{
    if (immediate) {
        if (HAL_ADC_Stop_DMA(&hadc1) != HAL_OK) {
            Error_Handler();
        }
    }
    keep_converting = false;
}

void therm_ADCdone(void)
{
    vout_readings[vout_index] = (float)pending_readings[0];
    vout_index += 1;
    if (vout_index >= NUM_READINGS) {
        vout_index = 0;
    }
    vref_readings[vref_index] = (float)pending_readings[1];
    vref_index += 1;
    if (vref_index >= NUM_READINGS) {
        vref_index = 0;
    }
    if (keep_converting) {
        // setup another round of readings
        therm_ADC_start(false);
        if (!reading_ready && vref_index == 0) {
            // We've had enough readings!
            reading_ready = true;
        }
    } else {
        // Only one reading. Ready to read
        reading_ready = true;
        ADC_running = false;
    }
}

bool therm_valueReady(void) {
    return reading_ready;
}

bool therm_ADCRunning(void) {
    return ADC_running;
}

/*! Gets a reading from the thermocouple, returning
 *  the value in celsius.
 */
float therm_getValue_averaged(void)
{
    float sensor = 0;
    float v1_25 = 0;
    // Turn on the thermocouple and wait for it to come on

    // Gather some readings and average them
    for (int i = NUM_READINGS; i > 0; i--) {
        sensor += vout_readings[i];
        v1_25 += vref_readings[i];
    }

    // Normalize and send out
    sensor = sensor / NUM_READINGS;
    v1_25 = v1_25 / NUM_READINGS;
    float vout = sensor * (1.25 / v1_25);
    return (vout - 1.25) / 0.005;
}

/*! Gets a reading from the thermocouple, returning
 *  the value in celsius. No averaging!
 */
float therm_getValue_single(void)
{
    float sensor = vout_readings[THERM_GET_IDEX()];
    float v1_25 = vref_readings[THERM_GET_IDEX()];

    // Normalize and send out
    float vout = sensor * (1.25 / v1_25);
    return (vout - 1.25) / 0.005;
}
