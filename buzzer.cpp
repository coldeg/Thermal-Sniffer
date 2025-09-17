#include <Arduino.h>
#include <math.h>

#include "buzzer.h"

// Local state for buzzer tone computation
static float tonal = 440;
static const float pitch = 1.059463119;
static int variaNote = 1;
static int counter = 1;

void handleBuzzer(float diffTemp, float tempCorr, float threshold) {
  // External variables maintained in the main sketch
  extern int treshCount;
  extern float treshTemp;
  extern double treshPress;
  extern double pressureP;
  extern float temperatureP;
  extern double diffPress;

  if (treshCount < 2) {
    treshTemp = temperatureP;
    treshPress = pressureP;
    treshCount = 2;
  }

  diffPress = (treshPress - pressureP);
  tempCorr = diffPress * 0.09411616;

  if ((abs(diffTemp) - threshold) > 0) {
    if (diffTemp > 0.0) {
      counter = 2;
      variaNote = abs(treshTemp - temperatureP + tempCorr) * 5;
      tonal = 440 * pow(pitch, variaNote);
      tone(6, tonal, 1000);
    }
    if (-diffTemp > 0.0) {
      counter = 2;
      variaNote = (1 - abs(treshTemp - temperatureP + tempCorr)) * 5;
      tonal = 440 / pow(pitch, variaNote);
      tone(6, tonal, 1000);
    }
  }

  if ((abs(diffTemp) - threshold) < 0) {
    counter = counter + 1;
    if (counter < 20) {
      tone(6, tonal, 1000); // reset tonal if stagnation
    }
    if (counter > 20) {
      treshCount = 1;
      counter = 22;
    }
  }
}

