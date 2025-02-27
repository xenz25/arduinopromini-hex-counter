#include "Utils.h"

uint8_t getIndexFrom(uint8_t *list, uint8_t listLen, uint8_t data) {
  for (uint8_t i = 0; i < listLen; i++) {
    if (list[i] == data) {
      return i;
    }
  }

  return -1;
}

uint8_t invertIndex(uint8_t MAX_COUNT, uint8_t index) {
  return MAX_COUNT - index - 1;
}