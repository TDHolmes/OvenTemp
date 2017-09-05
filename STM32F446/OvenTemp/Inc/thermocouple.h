/*!
 * @file    thermocouple.h
 * @author  Tyler Holmes
 * @date    2-Sept-2017
 * @brief   Interface to the thermocouple temperature sensor
 */
#pragma once

#include <stdbool.h>

void therm_init(void);
void therm_ADCdone(void);
float therm_getValue_averaged(void);
float therm_getValue_single(void);

// Utilities
inline float c2f(float celsius_data);

// Status functions
bool therm_ADCRunning(void);
bool therm_valueReady(void);
