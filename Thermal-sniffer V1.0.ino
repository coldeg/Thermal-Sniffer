#include "src/temperature_sensor.h"
#include "src/barometer.h"
#include "src/display.h"
#include "src/buzzer.h"
#include "src/potentiometer.h"

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
  displayValues(temperature, pressure);
  delay(1000);
}
