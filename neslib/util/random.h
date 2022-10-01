#ifndef EMPTYNES_NESLIB_UTIL_RANDOM_H
#define EMPTYNES_NESLIB_UTIL_RANDOM_H
#include <stdint.h>

#include "util/compiler.h"

void fastcall rand_seed(uint16_t seed);

uint16_t fastcall rand16();
uint8_t fastcall rand8(uint16_t seed);

#endif  // EMPTYNES_NESLIB_UTIL_RANDOM_H
