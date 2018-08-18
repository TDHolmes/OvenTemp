/*!
 * @file    main.c
 * @author  Tyler Holmes
 * @date    2-Sept-2017
 * @brief   Main file for the OvenTemp project.
 *
 *      Stupid STM copyright notice at end.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "hardware.h"
#include "display.h"
#include "thermocouple.h"
#include "stm32f4xx_hal.h"

#ifdef __APPLE__
    #define SECTION(X) section("__DATA,__" X )
    #define SECTION_START(X) __asm("section$start$__DATA$__" X)
    #define SECTION_END(X) __asm("section$end$__DATA$__" X)
#else
    #define SECTION(X) section(X)
    #define SECTION_START(X)
    #define SECTION_END(X)
#endif


//! How long we sleep between readings in idle mode, in seconds.
#define IDLE_MODE_SLEEPTIME    (60)
//! How long we sleep between readings in active mode, in seconds.
#define ACTIVE_MODE_SLEEPTIME  (1)

//! threshold to switch to active mode, in celsius
#define ACTIVE_TEMP_THRESHOLD   (50.0f)
#define INSANE_TEMP_THRESHOLD   (250.0f)

#define IDLE_SAMPLE_TIME_MS    (2000)  // TODO: change to 60 seconds
#define ACTIVE_SAMPLE_TIME_MS  (1000)

#define IDLE_BLINK_PERIOD_MS    (1000)
#define IDLE_BLINK_DURATION_MS  (500)

//! Toggle heartbeat LED every 30 seconds
#define HB_TICK_TIME_MS (30000)
//! heartbeet LED is on for 1 seconds
#define HB_ON_TIME_MS   ( 1000)


ADC_HandleTypeDef hadc1;      //!< HAL handle for ADC1
DMA_HandleTypeDef hdma_adc1;  //!< HAL handle for DMA for ADC1. Currently not used.
I2C_HandleTypeDef hI2C3;      //!< HAL handle for I2C3 (display)
RTC_HandleTypeDef hrtc;       //!< HAL handle for our RTC. Used to awake us from deep sleep
UART_HandleTypeDef huart4;    //!< HAL handle for UART4 for debug prints

extern __IO uint32_t uwTick;  //!< HAL tick count


//! Different modes the main loop can be in based on the oven temperature.
typedef enum {
    kIdleMode,
    kActiveMode,
    kInsaneTempMode,
    kInvalidMainMode
} e_main_modes;

static e_main_modes mode = kIdleMode;  //!< global tracking main's state
static uint8_t str_buff[64];  //!< buffer for transmitting data over UART

/****  Private function definitions  ****/
void blocking_delay(volatile uint32_t delay);
void blinkLED_withDelay(uint32_t delay);
void displayTemp(float temp, bool inFarenheit);
void sleep_enterSleep(void);
void sleep_enterStop(void);

// Modes
void idleMode(void);
void activeMode(void);
void errMode(char * err_reason);


#ifdef DEBUG
    void _print_string(char string[]);
    #define print_string(x) _print_string(x)
#else
    #define print_string(x)  /* Don't do anything. */
#endif


/*! Main function. Initializes all peripherals needed, get's an initial thermocouple
 *   reading, then goes into either active or idle mode. We then continue, checking
 *   the battery voltage
 */
int main(void)
{
    //! Reset of all peripherals, Initializes the Flash interface and the Systick.
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    hw_GPIO_Init();

    #ifdef DEBUG
        hw_UART4_Init();
    #endif

    print_string("Hello World!\n");

    hw_ADC1_Init();
    hw_I2C3_Init();
    hw_RTC_Init();

    while (true) {
        print_string("beep boop\n");
        HAL_Delay(1000);
    }

    /* Initialize interrupts */
    hw_NVIC_Init();

    // Initialize display and clear all digits
    disp_init(DISP_I2C_ADDR);
    disp_setBrightness(0);  // Lowest brightness
    disp_writeDigit_ascii(0, ' ', false);
    disp_writeDigit_ascii(1, 'H', false);
    disp_writeDigit_ascii(2, 'I', false);
    disp_writeDigit_ascii(3, ' ', false);
    disp_writeDisplay();

    HAL_Delay(1000);

    disp_writeDigit_raw(0, 0);
    disp_writeDigit_raw(1, 0);
    disp_writeDigit_raw(2, 0);
    disp_writeDigit_raw(3, 0);
    disp_writeDisplay();

    therm_init();

    //  Main infinite loop
    print_string("Entering Main\n");
    therm_startReading_single();
    while (1) {
        switch (mode) {
            case kIdleMode:
                print_string("kIdleMode\n");
                idleMode();
                break;

            case kActiveMode:
                print_string("kActiveMode\n");
                activeMode();
                break;

            case kInsaneTempMode:
                print_string("kInsaneTempMode\n");
                // Write the reason for the error
                errMode("TEMP");
                if ( therm_valueReady() ) {
                    if ( therm_getValue_single() < INSANE_TEMP_THRESHOLD ) {
                        mode = kActiveMode;
                    }
                } else if ( !therm_ADCRunning() ) {
                    // Kick off a reading if there isn't one running
                    therm_startReading_single();
                }
                break;

            case kInvalidMainMode:
                print_string("kInvalidMainMode\n");
                // Write the reason for the error
                errMode("MAIN");
                break;

            default:
                print_string("kDefaultMainMode\n");
                errMode("UNKN");
                break;
        }
    }
}


void activeMode(void)
{
    float temperature = 0;
    static uint32_t time_for_reading = 0;

    if ( therm_valueReady() && HAL_GetTick() >= time_for_reading ) {
        temperature = therm_getValue_averaged();

        // sprintf((char *)str_buff, "active temp: %f\n", temperature);
        // print_string((char *)str_buff);

        if (temperature < ACTIVE_TEMP_THRESHOLD) {
            disp_clear();
            disp_writeDisplay();
            mode = kIdleMode;
            time_for_reading = HAL_GetTick() + IDLE_SAMPLE_TIME_MS;
            therm_startReading_single();  // Single thermocouple conversion
        } else if (temperature > INSANE_TEMP_THRESHOLD) {
            mode = kInsaneTempMode;
        } else {
            // temperature at needed value. Display temp
            time_for_reading = HAL_GetTick() + ACTIVE_SAMPLE_TIME_MS;
            displayTemp(temperature, true);  // display temp in in farenheit
            therm_startReading_single();  // Single thermocouple conversion
            // deep sleep for a second to save power
            // TODO: switch back to standby
            sleep_enterSleep();
        }
    } else if ( !therm_ADCRunning() ) {
        // If we're not running temp readings, do that!
        therm_startReading_single();
    } else {
        // Not done yet... Keep snoozin! ADC interrupt should wake us from SLEEP
        sleep_enterSleep();
    }
}


void idleMode(void)
{
    float temperature;
    static uint32_t time_for_reading = 0;
    static uint32_t time_for_blink = 0;
    static bool blink_on = false;

    if ( therm_valueReady() && HAL_GetTick() >= time_for_reading ) {
        temperature = therm_getValue_single();

        if ( temperature >= ACTIVE_TEMP_THRESHOLD ) {
            time_for_reading = HAL_GetTick() + ACTIVE_SAMPLE_TIME_MS;
            mode = kActiveMode;
            therm_startReading_single();  // Single thermocouple conversion
        } else {
            time_for_reading = HAL_GetTick() + IDLE_SAMPLE_TIME_MS;
            // Configure the RTC to wake us up in N miliseconds from standby
            hw_RTC_setWakeup(IDLE_SAMPLE_TIME_MS);
            sleep_enterStop();
            // Start a single thermocouple read after we wake back up
            therm_startReading_single();
        }
    } else if ( !therm_ADCRunning() ) {
        // If we're not running temp readings, do that!
        therm_startReading_single();
    } else {
        // Not done yet... Keep snoozin! ADC interrupt should wake us from SLEEP
        sleep_enterSleep();
    }

    // Check up on the blinker
    if ( HAL_GetTick() >= time_for_blink ) {
        if ( blink_on == true ) {
            disp_writeDigit_ascii(3, ' ', false);  // Turn off the blink
            disp_writeDisplay();
            blink_on = false;
            time_for_blink = HAL_GetTick() + IDLE_BLINK_PERIOD_MS;  // 30 seconds between blinks
        } else {
            disp_writeDigit_ascii(3, ' ', true);  // Turn on the blink
            disp_writeDisplay();
            blink_on = true;
            time_for_blink = HAL_GetTick() + IDLE_BLINK_DURATION_MS;  // 500 ms blink duration
        }
    }
}


typedef enum {
    kErrWriteReason,
    kErrWriteErr
} err_mode_state_t;

void errMode(char * err_reason)
{
    static err_mode_state_t err_mode = kErrWriteReason;
    static uint32_t time_for_err_display = 0;

    if ( time_for_err_display < HAL_GetTick() ) {
        if (err_mode == kErrWriteReason) {
            for (uint8_t i = 0; i < 4; i++) {
                disp_writeDigit_ascii(i, err_reason[i], false);
            }
            err_mode = kErrWriteErr;
        } else {
            disp_writeDigit_ascii(0, 'E', false);
            disp_writeDigit_ascii(1, 'R', false);
            disp_writeDigit_ascii(2, 'R', false);
            disp_writeDigit_ascii(3, '!', false);
            err_mode = kErrWriteReason;
        }
        disp_writeDisplay();
        time_for_err_display = HAL_GetTick() + 1000;
    }
    sleep_enterSleep();
}


void sleep_enterSleep(void)
{
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}


void sleep_enterStop(void)
{
    // TODO: Confirm if the RTC is generating an EXTI interrupt or an event.
    //   I think I'm configuring it for an event, but it's ambiguous
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFE);
}


/*! This function takes in a positive, floating point value from [0, 1000) and
 *   displays it on the four digit display we have with the most percision possible.
 */
void displayTemp(float temp, bool inFarenheit)
{
    uint8_t a,b,c,d = 0;

    if (inFarenheit) {
        temp = temp * 1.8f + 32.0f;
    }
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


void HAL_IncTick(void)
{
    static uint32_t HB_time = HB_TICK_TIME_MS;
    static bool HB_on = false;

    // Handle the heartbeat LED
    if (uwTick >= HB_time) {
        if (HB_on == false) {
            hw_LED_setValue(1);
            HB_time = uwTick + HB_ON_TIME_MS;
            HB_on = true;
        } else {
            hw_LED_setValue(0);
            HB_time = uwTick + HB_TICK_TIME_MS;
            HB_on = false;
        }
    }

    // Increment ms tick
    uwTick++;
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
    _Error_Handler_withRetval(file, line, 0);
}


#define SOS_O  (300000)
#define SOS_S  (150000)


/*! Helper function that blinks our LED with delays before and after using
 *  `blocking_delay`.
 */
void blinkLED_withDelay(uint32_t delay) {
    hw_LED_setValue(0);
    blocking_delay(delay);
    hw_LED_setValue(1);
    blocking_delay(delay);
}

/*! This function is called when there is a fatal error somewhere in the code.
 *   The current behavior is to blink rapidly and send a formatted string over
 *   UART.
 * @param  file (char *): Name of the file in which the error occurred.
 * @param  line (int): Line number on which the error occurred.
 * @param  retval (int): The error code for the failure.
 */
void _Error_Handler_withRetval(char * file, int line, int retval)
{
    sprintf((char *)str_buff, "%s:%i  ->  %i\n", file, line, retval);
    int cycle = 0;
    typedef enum { kSave, kOur, kSouls, kPause} eSOS;
    eSOS sos_state = kSave;
    while(1)
    {
        cycle += 1;
        switch (sos_state) {
            case kSave:
                if (cycle >= 3) {
                    sos_state = kOur;
                    cycle = 0;
                }
                blinkLED_withDelay(SOS_S);
                break;

            case kOur:
                if (cycle >= 3) {
                    sos_state = kSouls;
                    cycle = 0;
                }
                blinkLED_withDelay(SOS_O);
                break;

            case kSouls:
                if (cycle >= 3) {
                    sos_state = kPause;
                    cycle = 0;
                }
                blinkLED_withDelay(SOS_S);
                break;

            case kPause:
                hw_LED_setValue(0);
                blocking_delay(SOS_O * 3);
                cycle = 0;
                sos_state = kSave;
                break;
        }

        // Print the error once a "cycle"
        if (sos_state == kPause) {
            print_string(str_buff);
        }
    }
}



#ifdef DEBUG
    void _print_string(char string[]) {
        HAL_UART_Transmit(&huart4, (uint8_t *)string,
                          strlen((const char *)string), 10);
    }
#endif


/*! Simple, dumb delay function to be used when we can't necessarily rely on
 *   interrupts to do delay timing. Normally, you should use interrupt driven or
 *   HAL_Delay() to rely on the systick.
 */
inline void blocking_delay(volatile uint32_t delay) {
    for (; delay != 0; delay--);
}


// Index for doxygen
/*! \mainpage Documentation for OvenTemp project!
 *
 * \section intro_sec Introduction
 *
 * This project is a simple thermocouple with display to easily see the temperature
 * of an old oven that doesn't have a digital display. It is designed to be battery
 * powered and smart enough to only turn on when needed. It uses an STM32f446
 * microcontroller because that was the easiest development board I had laying
 * around to use. It is also quite nice to work with and has good low power
 * performance.
 *
 */


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
