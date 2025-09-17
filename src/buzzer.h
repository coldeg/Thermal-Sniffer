/**
 * @file buzzer.h
 * @brief Generates audio feedback based on the filtered thermal indicator.
 *
 * The buzzer emits tones when the smoothed thermal signal exceeds the user
 * defined threshold. A small hysteresis prevents rapid toggling around the
 * trigger point.
 */
#pragma once

void buzzerSetup();
void buzzerUpdate(float filteredSignal, float threshold);
