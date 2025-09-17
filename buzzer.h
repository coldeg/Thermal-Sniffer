#ifndef BUZZER_H
#define BUZZER_H

// Handle the buzzer output depending on the temperature difference.
// diffTemp   : temperature difference between successive measurements
// tempCorr   : temperature correction based on pressure difference
// threshold  : sensitivity threshold (potentiometer value / 10)
void handleBuzzer(float diffTemp, float tempCorr, float threshold);

#endif // BUZZER_H

