/**
 * @file temperature_sensor.cpp
 * @brief Implementation of the MCP9808 temperature sensor module.
 */

#include "temperature_sensor.h"
#include <Wire.h>
#include <Arduino.h>

#define MCP9808_ADDR 0x18

void temperatureSensorSetup() {
    Wire.begin();
}

float readTemperatureC() {
    Wire.beginTransmission(MCP9808_ADDR);
    Wire.write(0x05); // temperature register
    Wire.endTransmission();
    Wire.requestFrom(MCP9808_ADDR, 2);
    if (Wire.available() != 2) {
        return 0.0f;
    }
    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();
    int16_t raw = ((msb & 0x1F) << 8) | lsb;
    if (raw & 0x1000) { // sign bit
        raw -= 0x2000;
    }
    return raw * 0.0625f;
}
