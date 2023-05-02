#include <Arduino.h>
#include "Random.h"

void Random::seedRandom(uint8_t analogPin) {
  pinMode(analogPin, INPUT);
  randomSeed(analogRead(analogPin));
}

void Random::string(char *result, size_t resultSize, bool alpha, bool numeric, bool punctuation, bool space,
                    bool randomCase) {
  for (size_t i = 0; i < resultSize;) {
    const char c = (char)random(32, 127);
    if ((isAlpha(c) && alpha) ||
        (isDigit(c) && numeric) ||
        (isPunct(c) && punctuation) ||
        (c == ' ' && space)) {
      if (!randomCase) {
        result[i] = (char)tolower(c);
      } else {
        result[i] = c;
      }
      i++;
    } else {
      continue;
    }
  }
}
