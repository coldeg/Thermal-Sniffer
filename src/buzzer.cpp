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

void buzzerUpdate(float filteredSignal, float threshold) {
    static bool buzzerActive = false;
    const int buzzerPin = 6;
    const float HYSTERESIS_FACTOR = 0.85f;

    float thresholdOn = threshold;
    float thresholdOff = threshold * HYSTERESIS_FACTOR;
    if (thresholdOff < 0.0f) {
        thresholdOff = 0.0f;
    }

    float signal = filteredSignal;
    if (signal < 0.0f) {
        signal = 0.0f;
    }

    if (buzzerActive) {
        if (signal <= thresholdOff) {
            noTone(buzzerPin);
            buzzerActive = false;
            return;
        }
    } else if (signal >= thresholdOn) {
        buzzerActive = true;
    } else {
        noTone(buzzerPin);
        return;
    }

    float normalized = signal - thresholdOff;
    if (normalized < 0.0f) {
        normalized = 0.0f;
    }

    int variaNote = constrain(static_cast<int>(normalized * 12.0f), 0, 48);
    float tonal = tonalBase * pow(pitch, variaNote);
    tone(buzzerPin, tonal, 120);
}
