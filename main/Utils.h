#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

// calculate array length
#define arrLength(arr) sizeof(arr) / sizeof(arr[0])

uint8_t getIndexFrom(uint8_t *list, uint8_t listLen, uint8_t data);

uint8_t invertIndex(uint8_t MAX_COUNT, uint8_t index);

#endif