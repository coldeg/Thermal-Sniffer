/**
 * @file display.cpp
 * @brief E-paper display rendering module.
 */

#include "display.h"
#include <Arduino.h>
#include <epd2in9.h>
#include <epdpaint.h>

#define COLORED 0
#define UNCOLORED 1

static unsigned char image[128 * 24 / 8];
static Paint paint(image, 128, 24);
static Epd epd;

void displaySetup() {
    if (epd.Init(lut_full_update) != 0) {
        return;
    }
    epd.ClearFrameMemory(0xFF);
    epd.DisplayFrame();
    epd.ClearFrameMemory(0xFF);
    epd.DisplayFrame();
    paint.SetRotate(ROTATE_0);
}

void displayValues(float temperature, double pressure) {
    char tempBuf[8];
    dtostrf(temperature, 6, 2, tempBuf);
    paint.Clear(UNCOLORED);
    paint.DrawStringAt(0, 4, tempBuf, &Font12, COLORED);
    epd.SetFrameMemory(paint.GetImage(), 0, 10, paint.GetWidth(), paint.GetHeight());

    char presBuf[8];
    dtostrf(pressure, 6, 2, presBuf);
    paint.Clear(UNCOLORED);
    paint.DrawStringAt(0, 4, presBuf, &Font12, COLORED);
    epd.SetFrameMemory(paint.GetImage(), 0, 30, paint.GetWidth(), paint.GetHeight());
    epd.DisplayFrame();
}
