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
#include <WiFiClientSecure.h>
#include <Random.h>
#include "proxy.h"

JPEGDEC jpeg;
const size_t JPEG_BUF_MAX_SIZE = 32768;
uint8_t jpegBuf[JPEG_BUF_MAX_SIZE] = {};

int JPEGDraw(JPEGDRAW *pDraw) {
  int x = pDraw->x;
  int y = pDraw->y;
  int w = pDraw->iWidth;
  int h = pDraw->iHeight;
  
  for(int i = 0; i < w * h; i ++) {
    pDraw->pPixels[i] = (pDraw->pPixels[i] & 0x7E0) >> 5;
  }
  
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      const uint16_t px = pDraw->pPixels[i + j * w];
      if (px < 32) {
        display.writePixel(x + i, y + j, GxEPD_BLACK); // NOLINT(cppcoreguidelines-narrowing-conversions)
      } else {
        display.writePixel(x + i, y + j, GxEPD_WHITE); // NOLINT(cppcoreguidelines-narrowing-conversions)
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
  display.hibernate();
  
  WiFi.mode(WIFI_STA); // NOLINT(readability-static-accessed-through-instance)
  WiFiManager wm;
//  wm.resetSettings();

  while (true) {
    display.fillRect(0, 201, display.width(), 9 + padding * 2, GxEPD_WHITE);
    display.setCursor(0 + padding, 201 + padding);
    if (!wm.autoConnect(apName, apPwd)) {
      display.print("Failed to connect, try again!");
      display.display();
      display.hibernate();
    } else {
      display.print("Connected successfully!");
      display.display();
      display.hibernate();
      break;
    }
  }
}

[[noreturn]] void handleTCECGameDisplay() {
  display.fillScreen(GxEPD_WHITE);
  
  const uint8_t padding = 2;
  
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(0 + padding, 9 + padding);
  display.print("TCEC - Live Computer Chess Broadcast");
  
  display.setFont(&FreeMono9pt7b);
  
  WiFiClientSecure client;
  client.setCACert(SERVER_PROXY_ROOT_CERT);
  if (client.connect(SERVER_PROXY, 443)) {
    Serial.print("Connected successfully to ");
    Serial.print(SERVER_PROXY);
    Serial.println(" on port 443");
    client.println("GET /image.jpg?size=250 HTTP/1.1");
    client.print("Host: ");
    client.println(SERVER_PROXY);
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
    Serial.println("Waiting for response");
    while (client.available() == 0) {
      delay(5);
    }
    Serial.println("Response streaming start");
    size_t jpegLen = 0;
    if (!client.find("\r\n\r\n")) {
      Serial.println("Could not find end of headers!");
      display.print("Unable to parse response!");
    } else {
      while (client.connected() && jpegLen < JPEG_BUF_MAX_SIZE) {
        if (client.available() > 0) {
//          Serial.print(client.peek());
//          Serial.print(" ");
//          Serial.write(client.peek());
          jpegBuf[jpegLen] = client.read();
          jpegLen ++;
        }
      }
      Serial.print("Response streaming end, received ");
      Serial.print(jpegLen);
      Serial.println(" bytes");
      client.stop();
      Serial.println("Stopping client");
      display.setCursor(0 + padding, 25 + padding);
      if (jpeg.openRAM(jpegBuf, (int16_t)jpegLen, JPEGDraw)) {
        Serial.println("Successfully parsed JPEG headers");
        if (jpeg.decode(0 + padding, 15 + padding, 0)) {
          Serial.println("Successfully decoded JPEG");
        } else {
          Serial.println("Failed to decode JPEG");
          display.print("Failed to decode JPEG!");
        }
        jpeg.close();
      } else {
        Serial.println("Failed to parse JPEG headers");
        display.print("Failed to parse JPEG!");
      }
    }
  } else {
    Serial.println("Unable to connect to proxy!");
    display.print("Unable to connect to proxy!");
  }
  
  display.display();
  
  display.hibernate();
  
  while (true) {
  
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
  
  delay(1000);
  
  handleWifiConnection();
  
  delay(1000);
}

void loop() {
  handleTCECGameDisplay();
}
