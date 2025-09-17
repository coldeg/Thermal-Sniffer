/**
 * @file buzzer.cpp
 * @brief Implementation of the buzzer feedback module.
 */

#include "buzzer.h"
#include <Arduino.h>
#include <math.h>

static const float tonalBase = 440.0f;
static const float pitch = 1.059463119f;

void buzzerSetup() {}

void buzzerUpdate(float tempDiff, float threshold) {
    if (fabs(tempDiff) > threshold) {
        int variaNote = abs(tempDiff) * 5;
        float tonal = tonalBase;
        if (tempDiff > 0) {
            tonal *= pow(pitch, variaNote);
        } else {
            tonal /= pow(pitch, variaNote);
        }
        tone(6, tonal, 1000);
    }
}
