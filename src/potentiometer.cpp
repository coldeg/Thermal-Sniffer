/**
 * @file potentiometer.cpp
 * @brief Implementation of the potentiometer threshold module.
 */

#include "potentiometer.h"
#include <Arduino.h>

static int potPin = 0;

void potentiometerSetup(int pin) {
    potPin = pin;
    pinMode(potPin, INPUT);
}

float readPotentiometer() {
    float pot = analogRead(potPin) / 100.0f;
    return pot / 10.0f + 0.6f;
}
