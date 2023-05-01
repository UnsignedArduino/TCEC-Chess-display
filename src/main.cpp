// https://github.com/bitbank2/JPEGDEC/blob/master/examples/epd_demo/epd_demo.ino

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMono9pt7b.h>
#include "GxEPD2_display_selection_new_style.h"
#include <JPEGDEC.h>

JPEGDEC jpeg;

int JPEGDraw(JPEGDRAW *pDraw) {
  int x = pDraw->x;
  int y = pDraw->y;
  int w = pDraw->iWidth;
  int h = pDraw->iHeight;
  
  for(int i = 0; i < w * h; i++) {
    pDraw->pPixels[i] = (pDraw->pPixels[i] & 0x7e0) >> 5; // extract just the six green channel bits.
  }
  
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      int8_t oldPixel = constrain(pDraw->pPixels[i + j * w], 0, 0x3F);
      // or 0x30 to dither to 2-bit directly. much improved tonal range, but more horizontal banding between blocks.
      int8_t newPixel = oldPixel & 0x38; // NOLINT(cppcoreguidelines-narrowing-conversions)
      pDraw->pPixels[i + j * w] = newPixel; // NOLINT(cert-str34-c)
      int quantError = oldPixel - newPixel;
      if (i + 1 < w) pDraw->pPixels[i + 1 + j * w] += quantError * 7 / 16;
      if ((i - 1 >= 0) && (j + 1 < h)) pDraw->pPixels[i - 1 + (j + 1) * w] += quantError * 3 / 16;
      if (j + 1 < h) pDraw->pPixels[i + (j + 1) * w] += quantError * 5 / 16;
      if ((i + 1 < w) && (j + 1 < h)) pDraw->pPixels[i + 1 + (j + 1) * w] += quantError * 1 / 16;
    }
  }
  
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      switch (constrain(pDraw->pPixels[i + j * w] >> 5, 0, 1)) {
        case 0:
          display.writePixel(x + i, y + j, GxEPD_BLACK); // NOLINT(cppcoreguidelines-narrowing-conversions)
          break;
        case 1:
          display.writePixel(x + i, y + j, GxEPD_WHITE); // NOLINT(cppcoreguidelines-narrowing-conversions)
          break;
      }
    }
  }
  return 1;
}

void setup() {
  Serial.begin(9600);
  display.init(9600, true, 2, false);
  display.setRotation(0);
  display.setFont(&FreeMono9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();
  display.fillScreen(GxEPD_WHITE);
  display.display(false);
  
  display.fillScreen(GxEPD_WHITE);
  display.display(false);
  display.powerOff();
}

void loop() {
}
