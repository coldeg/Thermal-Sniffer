/**
 * @file potentiometer.cpp
 * @brief Implementation of the potentiometer threshold module.
 */

#include "potentiometer.h"
#include <Arduino.h>

namespace {
struct PotentiometerConfig {
    float minGradientCPerS;
    float maxGradientCPerS;
    uint8_t smoothingSamples;
};

static PotentiometerConfig config = {0.01f, 0.30f, 8};
static int potPin = 0;
}

void potentiometerSetup(int pin, PotentiometerScale scale, uint8_t smoothingSamples) {
    potPin = pin;

    if (scale.maxGradientCPerS < scale.minGradientCPerS) {
        float temp = scale.maxGradientCPerS;
        scale.maxGradientCPerS = scale.minGradientCPerS;
        scale.minGradientCPerS = temp;
    }

    config.minGradientCPerS = scale.minGradientCPerS;
    config.maxGradientCPerS = scale.maxGradientCPerS;
    config.smoothingSamples = smoothingSamples == 0 ? 1 : smoothingSamples;

    pinMode(potPin, INPUT);
}

float readPotentiometer() {
    uint32_t total = 0;
    for (uint8_t i = 0; i < config.smoothingSamples; ++i) {
        total += static_cast<uint32_t>(analogRead(potPin));
    }

    float averageReading = static_cast<float>(total) / config.smoothingSamples;
    float normalized = constrain(averageReading / 1023.0f, 0.0f, 1.0f);

    float range = config.maxGradientCPerS - config.minGradientCPerS;
    return config.minGradientCPerS + normalized * range;
}
