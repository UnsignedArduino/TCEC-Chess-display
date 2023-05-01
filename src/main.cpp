#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMono9pt7b.h>
#include "GxEPD2_display_selection_new_style.h"

void setup() {
  Serial.begin(9600);
  display.init(9600, true, 2, false);
  display.setRotation(0);
  display.setFont(&FreeMono9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();
  display.fillScreen(GxEPD_WHITE);
  display.setCursor(0, 10);
  display.print("Hello, world!");
  display.display(false);
  delay(5000);
  display.fillScreen(GxEPD_WHITE);
  display.display(false);
  display.powerOff();
}

void loop() {
}
