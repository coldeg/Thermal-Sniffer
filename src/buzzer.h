/**
 * @file buzzer.h
 * @brief Generates audio feedback based on temperature variation.
 *
 * The buzzer emits tones whose pitch varies with the magnitude of the
 * temperature change.
 */
#pragma once

void buzzerSetup();
void buzzerUpdate(float tempDiff, float threshold);
