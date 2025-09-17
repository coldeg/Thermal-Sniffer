/**
 * @file potentiometer.h
 * @brief Provides sensitivity threshold from a potentiometer.
 *
 * This module reads an analog pin connected to a potentiometer to determine
 * the user-defined threshold for buzzer activation.
 */
#pragma once

void potentiometerSetup(int pin);
float readPotentiometer();
