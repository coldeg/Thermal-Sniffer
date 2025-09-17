#include "src/temperature_sensor.h"
#include "src/barometer.h"
#include "src/display.h"
#include "src/buzzer.h"
#include "src/potentiometer.h"

#include <math.h>

void setup() {
  Serial.begin(9600);
  temperatureSensorSetup();
  barometerSetup();
  displaySetup();
  buzzerSetup();
  potentiometerSetup(2);
}

void loop() {
  float temperature = readTemperatureC();
  double pressure = readPressure();
  static float lastTemp = temperature;
  float diff = temperature - lastTemp;
  lastTemp = temperature;
  float threshold = readPotentiometer();
  buzzerUpdate(diff, threshold);

  static float lastDisplayedTemp = NAN;
  static double lastDisplayedPressure = NAN;
  static unsigned long lastDisplayUpdate = 0;
  const float TEMP_TOLERANCE = 0.1f;
  const double PRESSURE_TOLERANCE = 0.5;
  const unsigned long DISPLAY_TIMEOUT_MS = 10000;

  unsigned long now = millis();
  bool firstUpdate = isnan(lastDisplayedTemp) || isnan(lastDisplayedPressure);
  bool tempChanged = firstUpdate || fabsf(temperature - lastDisplayedTemp) > TEMP_TOLERANCE;
  bool pressureChanged = firstUpdate || fabs(pressure - lastDisplayedPressure) > PRESSURE_TOLERANCE;
  bool timeoutElapsed = firstUpdate || (now - lastDisplayUpdate) >= DISPLAY_TIMEOUT_MS;

  if (tempChanged || pressureChanged || timeoutElapsed) {
    displayValues(temperature, pressure);
    lastDisplayedTemp = temperature;
    lastDisplayedPressure = pressure;
    lastDisplayUpdate = now;
  }

  delay(1000);
}
