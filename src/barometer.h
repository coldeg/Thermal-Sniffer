/**
 * @file barometer.h
 * @brief Reads pressure data from the MS5611 barometer.
 *
 * The module wraps the MS5611 driver providing initialization and a function
 * returning the current pressure in millibar.
 */
#pragma once

void barometerSetup();
double readPressure();
