/*!
 * @file    thermocouple.c
 * @author  Tyler Holmes
 * @date    2-Sept-2017
 * @brief   Interface to the thermocouple temperature sensor
 */

#include <stdbool.h>
#include <stdint.h>

#include "common.h"
#include "hardware.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
// #include "stm32f4xx_hal_dma.h"


#define NUM_READINGS  (8)  // TODO: justify
#define THERM_GET_IDEX()   (reading_index == (NUM_READINGS - 1) ? 0 : reading_index)

  //! HAL ADC handle to get readings
extern ADC_HandleTypeDef hadc1;

static float vout_readings[NUM_READINGS];  //!< circular buffer of readings for averaging
static float vref_readings[NUM_READINGS];  //!< circular buffer of readings for averaging

//! Where the ADC stores the current reading, via either DMA or interrupt
static uint32_t pending_readings[3];
//! The current, most valid index from the ADC stored in `vout_readings` or `vref_readings`
static uint8_t reading_index;
static bool keep_converting;  //!< tracks if we should continue triggering ADC conversions
static bool reading_ready;    //!< True if there is a valid reading available
static bool ADC_running;      //!< True if we are currently running the ADC peripheral


// Private function definitions
static void therm_ADC_done(void);
static void therm_startReading(bool single);

/*! Initializes all private variables for the thermocouple module to work.
 *    Does NOT start an ADC reading.
 */
void therm_init(void)
{
    for (int8_t i = NUM_READINGS - 1; i > 0; i--) {
        vout_readings[i] = 0;
        vref_readings[i] = 0;
    }
    pending_readings[0] = 0;
    pending_readings[1] = 0;
    pending_readings[2] = 0;
    reading_index = 0;
    reading_ready = false;
    ADC_running = false;
}

/*! Private function that either kicks off a single ADC reading or continuous
 *    readings. Called by the public functions `therm_startReading_single` and
 *    `therm_startReading_continuous`.
 */
static void therm_startReading(bool single_reading)
{
    // Start an ADC conversions for both channels
    if (HAL_ADC_Start_IT(&hadc1) != HAL_OK) {
        Error_Handler();
    }
    if (single_reading) {
        reading_ready = false;
    }
    if (keep_converting != !single_reading) {
        // This is our first time getting called for continuous readings. reset
        //   the `reading_index` to make sure we get all new values when averaging.
        reading_index = 0;
        keep_converting = true;
    }
    ADC_running = true;
}

/*! starts a single ADC reading.
 */
void therm_startReading_single(void) {
    therm_startReading(true);
}

/*! starts continuous ADC readings.
 */
void therm_startReading_continuous(void) {
    therm_startReading(false);
}

/*! Private function that gets called after the ADC is done converting all thermocouple
 *   values and pulls them from the `pending_readings` array. Also triggers another
 *   reading if the `keep_converting` flag is set.
 */
static void therm_ADC_done(void)
{
    vout_readings[reading_index] = (float)pending_readings[0];
    vref_readings[reading_index] = (float)pending_readings[1];
    reading_index += 1;
    if (reading_index >= NUM_READINGS) {
        reading_index = 0;
    }
    if (keep_converting) {
        // setup another round of readings
        therm_startReading_continuous();
        if (!reading_ready && reading_index == 0) {
            // We've had enough readings to get a valid, averaged temperature
            reading_ready = true;
        }
    } else {
        // Only one reading. Ready to read
        reading_ready = true;
        ADC_running = false;
    }
}

/*! Returns true if there is a valid reading ready, either single if not doing
 *   continuous readings, else enough to get an averaged result
 */
bool therm_valueReady(void) {
    return reading_ready;
}

/*! Returns true if the ADC is currently reading data in. Not currently used.
 */
bool therm_ADCRunning(void) {
    return ADC_running;
}

/*! Gets a reading from the thermocouple, returning
 *  the value in celsius with averaging over time.
 */
float therm_getValue_averaged(void)
{
    float sensor, v1_25, vout = 0;

    // Gather the readings and average them
    for (int8_t i = NUM_READINGS - 1; i >= 0; i--) {
        sensor += vout_readings[i];
        v1_25 += vref_readings[i];
    }

    // Normalize and send out
    sensor = sensor / NUM_READINGS;
    v1_25 = v1_25 / NUM_READINGS;
    vout = sensor * (1.25f / v1_25);
    return (vout - 1.25f) / 0.005f;
}

/*! Gets a reading from the thermocouple, returning
 *  the value in celsius. No averaging!
 */
float therm_getValue_single(void)
{
    float sensor = vout_readings[THERM_GET_IDEX()];
    float v1_25 = vref_readings[THERM_GET_IDEX()];

    // Normalize and send out
    float vout = sensor * (1.25f / v1_25);
    reading_ready = false;
    return (vout - 1.25f) / 0.005f;
}


/****** ADC Callback functions *********/


/*! The calback from the ADC interrupt that fires each time it converts
 *  a channel, then stores it in `pending_readings`. Also keeps track if all channels
 *  are done converting and modifies the flags `ADC_running`.
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    // Get the readings
    static uint8_t pending_index = 0;
    pending_readings[pending_index] = HAL_ADC_GetValue(hadc);
    pending_index++;
    if (pending_index == 3) {
        if (keep_converting) {
            // kick off another reading!
            if (HAL_ADC_Start_IT(&hadc1) != HAL_OK) {
                Error_Handler();
            }
        }
        // update state
        ADC_running = keep_converting;
        pending_index = 0;
        therm_ADC_done();
    } else {
        ADC_running = true;
        if (HAL_ADC_Start_IT(&hadc1) != HAL_OK) {
            Error_Handler();
        }
    }
}

/*! This function is called if the ADC peripheral runs into a critical error.
 */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef* hadc)
{
    Error_Handler_withRetval(__FILE__, __LINE__, hadc->ErrorCode);
}
