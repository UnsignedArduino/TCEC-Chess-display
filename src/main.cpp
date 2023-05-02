// https://github.com/bitbank2/JPEGDEC/blob/master/examples/epd_demo/epd_demo.ino

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "GxEPD2_display_selection_new_style.h"
#include <JPEGDEC.h>
#include <WiFi.h>
#include <WiFiType.h>
#include <WiFiManager.h>
#include <Random.h>

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

void handleWifiConnection() {
  display.fillScreen(GxEPD_WHITE);
  
  const uint8_t padding = 2;
  
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(0 + padding, 9 + padding);
  display.print("TCEC - Live Computer Chess Broadcast");
  
  display.setFont(&FreeMono9pt7b);
  display.setCursor(0 + padding, 25 + padding);
  display.print("Connect to WiFi");
  
  const char* apName = "TCECWifiSetup";
  const size_t AP_PWD_SIZE = 9;
  char apPwd[AP_PWD_SIZE];
  Random::string(apPwd, AP_PWD_SIZE, true, true, false, false, true);
  apPwd[AP_PWD_SIZE - 1] = '\0';

  display.setCursor(0 + padding, 57 + padding);
  display.print("Please connect to the following");
  display.setCursor(0 + padding, 73 + padding);
  display.print("network to configure WiFi:");
  display.setCursor(0 + padding, 105 + padding);
  display.print("AP name: "); display.print(apName);
  display.setCursor(0 + padding, 121 + padding);
  display.print("AP password: "); display.print(apPwd);
  display.setCursor(0 + padding, 153 + padding);
  display.print("Go to 192.168.4.1 if you are not");
  display.setCursor(0 + padding, 169 + padding);
  display.print("redirected automatically.");

  display.display();
  
  WiFi.mode(WIFI_STA);
  WiFiManager wm;
//  wm.resetSettings();

  while (true) {
    display.fillRect(0, 201, display.width(), 9 + padding * 2, GxEPD_WHITE);
    display.setCursor(0 + padding, 201 + padding);
    if (!wm.autoConnect(apName, apPwd)) {
      display.print("Failed to connect, try again!");
      display.display();
    } else {
      display.print("Connected successfully!");
      display.display();
      break;
    }
  }
}

void setup() {
  Serial.begin(9600);
  
  Random::seedRandom();

  display.init(9600, true, 2, false);
  display.setRotation(0);
  display.setFont(&FreeMono9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();
  display.fillScreen(GxEPD_WHITE);
  display.display(false);
  
  handleWifiConnection();
  
  delay(5000);
  
  display.hibernate();
}

void loop() {
}
