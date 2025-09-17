#include "src/temperature_sensor.h"
#include "src/barometer.h"
#include "src/display.h"
#include "src/buzzer.h"
#include "src/potentiometer.h"

#include <math.h>

static constexpr float LOOP_PERIOD_SECONDS = 1.0f;  // Matches the 1 s delay at the end of loop().
static const PotentiometerScale kGradientScale = {0.01f, 0.30f};
static constexpr uint8_t kPotSmoothingSamples = 8;

/**
 * @brief Converts the gradient-based potentiometer setting into a per-iteration delta.
 *
 * The potentiometer is configured to cover 0.01–0.30 °C/s (after adiabatic compensation or
 * an equivalent vertical speed range). The filter and hysteresis logic operate on the
 * temperature delta observed between two loop iterations, so we scale the gradient back to a
 * delta by multiplying it with the sampling period.
 */
static float gradientSettingToFilterDelta(float gradientCelsiusPerSecond) {
  return gradientCelsiusPerSecond * LOOP_PERIOD_SECONDS;
}

void setup() {
  Serial.begin(9600);
  temperatureSensorSetup();
  barometerSetup();
  displaySetup();
  buzzerSetup();
  potentiometerSetup(2, kGradientScale, kPotSmoothingSamples);
}

void loop() {
  float temperature = readTemperatureC();
  double pressure = readPressure();
  static float lastTemp = temperature;
  float diff = temperature - lastTemp;
  lastTemp = temperature;
  float gradientSetting = readPotentiometer();
  float threshold = gradientSettingToFilterDelta(gradientSetting);
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
