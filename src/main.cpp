// https://github.com/bitbank2/JPEGDEC/blob/master/examples/epd_demo/epd_demo.ino

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMono9pt7b.h>
#include "GxEPD2_display_selection_new_style.h"
#include <JPEGDEC.h>
#include "lange.h"

JPEGDEC jpeg;

const bool dither = true;

const uint8_t * images[] = {demo_001_jpg, demo_002_jpg, demo_003_jpg, demo_004_jpg, demo_005_jpg, demo_006_jpg, demo_007_jpg, demo_008_jpg};
size_t imageLengths[] = {demo_001_jpg_len, demo_002_jpg_len, demo_003_jpg_len, demo_004_jpg_len, demo_005_jpg_len, demo_006_jpg_len, demo_007_jpg_len, demo_008_jpg_len};


int JPEGDraw(JPEGDRAW *pDraw)
{
  int x = pDraw->x;
  int y = pDraw->y;
  int w = pDraw->iWidth;
  int h = pDraw->iHeight;
  
  for(int i = 0; i < w * h; i++)
  {
    pDraw->pPixels[i] = (pDraw->pPixels[i] & 0x7e0) >> 5; // extract just the six green channel bits.
  }
  
  if (dither)
  {
    for(int16_t j = 0; j < h; j++)
    {
      for(int16_t i = 0; i < w; i++)
      {
        int8_t oldPixel = constrain(pDraw->pPixels[i + j * w], 0, 0x3F);
        int8_t newPixel = oldPixel & 0x38; // or 0x30 to dither to 2-bit directly. much improved tonal range, but more horizontal banding between blocks.
        pDraw->pPixels[i + j * w] = newPixel;
        int quantError = oldPixel - newPixel;
        if(i + 1 < w) pDraw->pPixels[i + 1 + j * w] += quantError * 7 / 16;
        if((i - 1 >= 0) && (j + 1 < h)) pDraw->pPixels[i - 1 + (j + 1) * w] += quantError * 3 / 16;
        if(j + 1 < h) pDraw->pPixels[i + (j + 1) * w] += quantError * 5 / 16;
        if((i + 1 < w) && (j + 1 < h)) pDraw->pPixels[i + 1 + (j + 1) * w] += quantError * 1 / 16;
      } // for i
    } // for j
  } // if dither
  
  for(int16_t i = 0; i < w; i++)
  {
    for(int16_t j = 0; j < h; j++)
    {
      switch (constrain(pDraw->pPixels[i + j * w] >> 5, 0, 1))
      {
        case 0:
          display.writePixel(x+i, y+j, GxEPD_BLACK);
          break;
        case 1:
          display.writePixel(x+i, y+j, GxEPD_WHITE);
          break;
      } // switch
    } // for j
  } // for i
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
  
  int i;
  long lTime;
  
  for (i=0; i<8; i++)
  {
    if (jpeg.openFLASH((uint8_t *)images[i], imageLengths[i], JPEGDraw))
    {
      lTime = micros();
      if (jpeg.decode((300 - jpeg.getWidth()) / 2, (400 - jpeg.getHeight()) / 2, 0))
      {
        lTime = micros() - lTime;
        Serial.printf("%d x %d image, decode time = %d us\n", jpeg.getWidth(), jpeg.getHeight(), (int)lTime);
      }
      jpeg.close();
    }
    display.display();
    delay(4000); // pause between images
  } // for i
  
  display.fillScreen(GxEPD_WHITE);
  display.display(false);
  display.powerOff();
}

void loop() {
}
