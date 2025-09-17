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
  const unsigned long SAMPLE_INTERVAL_MS = 150;
  const unsigned long WINDOW_DURATION_MS = 5000;
  const float GROUND_SPEED_MPS = 11.11f; // ~40 km/h
  const size_t MAX_WINDOW_SAMPLES = WINDOW_DURATION_MS / SAMPLE_INTERVAL_MS + 4;

  static unsigned long lastSampleTime = 0;
  unsigned long now = millis();
  if (lastSampleTime != 0 && now - lastSampleTime < SAMPLE_INTERVAL_MS) {
    return;
  }
  lastSampleTime = now;

  float temperature = readTemperatureC();
  double pressure = readPressure();

  static float temperatureHistory[MAX_WINDOW_SAMPLES];
  static double pressureHistory[MAX_WINDOW_SAMPLES];
  static unsigned long timestampHistory[MAX_WINDOW_SAMPLES];
  static size_t windowStart = 0;
  static size_t windowEnd = 0;
  static size_t windowCount = 0;

  temperatureHistory[windowEnd] = temperature;
  pressureHistory[windowEnd] = pressure;
  timestampHistory[windowEnd] = now;
  windowEnd = (windowEnd + 1) % MAX_WINDOW_SAMPLES;
  if (windowCount < MAX_WINDOW_SAMPLES) {
    ++windowCount;
  } else {
    windowStart = (windowStart + 1) % MAX_WINDOW_SAMPLES;
  }

  while (windowCount > 0) {
    unsigned long oldestTime = timestampHistory[windowStart];
    if (now - oldestTime <= WINDOW_DURATION_MS) {
      break;
    }
    windowStart = (windowStart + 1) % MAX_WINDOW_SAMPLES;
    --windowCount;
  }

  float tempSlopeSum = 0.0f;
  float tempSlopeSqSum = 0.0f;
  float pressureSlopeSum = 0.0f;
  float pressureSlopeSqSum = 0.0f;
  size_t slopeCount = 0;

  if (windowCount > 1) {
    for (size_t i = 0; i < windowCount - 1; ++i) {
      size_t idxCurrent = (windowStart + i) % MAX_WINDOW_SAMPLES;
      size_t idxNext = (windowStart + i + 1) % MAX_WINDOW_SAMPLES;
      unsigned long dtMs = timestampHistory[idxNext] - timestampHistory[idxCurrent];
      if (dtMs == 0) {
        continue;
      }
      float dtSeconds = static_cast<float>(dtMs) / 1000.0f;
      float tempSlopePerMeter = ((temperatureHistory[idxNext] - temperatureHistory[idxCurrent]) / dtSeconds) / GROUND_SPEED_MPS;
      float pressureSlopePerMeter = ((pressureHistory[idxNext] - pressureHistory[idxCurrent]) / dtSeconds) / GROUND_SPEED_MPS;

      tempSlopeSum += tempSlopePerMeter;
      tempSlopeSqSum += tempSlopePerMeter * tempSlopePerMeter;
      pressureSlopeSum += pressureSlopePerMeter;
      pressureSlopeSqSum += pressureSlopePerMeter * pressureSlopePerMeter;
      ++slopeCount;
    }
  }

  float filteredSignal = 0.0f;
  if (slopeCount > 0) {
    float meanTempSlope = tempSlopeSum / slopeCount;
    float meanPressureSlope = pressureSlopeSum / slopeCount;

    float tempVariance = tempSlopeSqSum / slopeCount - meanTempSlope * meanTempSlope;
    tempVariance = tempVariance < 0.0f ? 0.0f : tempVariance;
    float tempStd = sqrtf(tempVariance);

    float pressureVariance = pressureSlopeSqSum / slopeCount - meanPressureSlope * meanPressureSlope;
    pressureVariance = pressureVariance < 0.0f ? 0.0f : pressureVariance;
    float pressureStd = sqrtf(pressureVariance);

    float coherentTemp = 0.0f;
    if (meanTempSlope > 0.0f) {
      coherentTemp = meanTempSlope - tempStd;
      if (coherentTemp < 0.0f) {
        coherentTemp = 0.0f;
      }
    }

    float coherentPressure = 0.0f;
    float pressureDrop = -meanPressureSlope;
    if (pressureDrop > 0.0f) {
      coherentPressure = pressureDrop - pressureStd;
      if (coherentPressure < 0.0f) {
        coherentPressure = 0.0f;
      }
    }

    filteredSignal = (coherentTemp + coherentPressure) * 100.0f;
  }

  float threshold = readPotentiometer();
  buzzerUpdate(filteredSignal, threshold);

  static float lastDisplayedTemp = NAN;
  static double lastDisplayedPressure = NAN;
  static unsigned long lastDisplayUpdate = 0;
  const float TEMP_TOLERANCE = 0.1f;
  const double PRESSURE_TOLERANCE = 0.5;
  const unsigned long DISPLAY_TIMEOUT_MS = 10000;

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
}
