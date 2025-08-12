/**
 * @file temperature_sensor.h
 * @brief Handles MCP9808 temperature sensor communication.
 *
 * This module initializes the sensor and provides a function to read the
 * current ambient temperature in degrees Celsius.
 */
#pragma once

void temperatureSensorSetup();
float readTemperatureC();
