#include <Arduino.h>

#ifndef TCEC_CHESS_DISPLAY_RANDOM_H
#define TCEC_CHESS_DISPLAY_RANDOM_H


namespace Random {
  void seedRandom(uint8_t analogPin = A0);
  void string(char* result, size_t resultSize, bool alpha, bool numeric, bool punctuation, bool space,
              bool randomCase);
};

#endif //TCEC_CHESS_DISPLAY_RANDOM_H
