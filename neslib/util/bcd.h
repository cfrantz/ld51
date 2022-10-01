#ifndef EMPTYNES_NESLIB_UTIL_BCD_H
#define EMPTYNES_NESLIB_UTIL_BCD_H
#include <stdint.h>
#include "util/compiler.h"

uint16_t fastcall bcd_add16(uint16_t a, uint16_t b);
uint16_t fastcall bcd_sub16(uint16_t a, uint16_t b);

#endif  // EMPTYNES_NESLIB_UTIL_BCD_H
