/**
 * @file potentiometer.h
 * @brief Provides a configurable gradient threshold from a potentiometer.
 *
 * This module reads an analog pin connected to a potentiometer to determine
 * a user-defined gradient threshold (e.g. 0.01–0.30 °C/s after adiabatic
 * compensation). The raw ADC value is mapped onto the configured scale and
 * optionally averaged across several samples for improved stability in flight.
 */
#pragma once

#include <stdint.h>

/**
 * @brief Describes the gradient range covered by the potentiometer.
 */
struct PotentiometerScale {
    float minGradientCPerS; /**< Threshold corresponding to the minimum ADC value. */
    float maxGradientCPerS; /**< Threshold corresponding to the maximum ADC value. */
};

void potentiometerSetup(int pin,
                        PotentiometerScale scale = {0.01f, 0.30f},
                        uint8_t smoothingSamples = 8);
/**
 * @brief Returns the configured gradient threshold in °C/s.
 */
float readPotentiometer();
