/**
 * @file barometer.cpp
 * @brief Implementation of the MS5611 barometer module.
 */

#include "barometer.h"
#include <MS5611.h>
#include <Arduino.h>

static MS5611 baro;

void barometerSetup() {
    baro = MS5611();
    baro.begin();
}

double readPressure() {
    return baro.getPressure();
}
