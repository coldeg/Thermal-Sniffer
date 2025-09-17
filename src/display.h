/**
 * @file display.h
 * @brief Controls the e-paper display.
 *
 * Responsible for initializing the display and rendering temperature and
 * pressure readings.
 */
#pragma once

void displaySetup();
void displayValues(float temperature, double pressure);
