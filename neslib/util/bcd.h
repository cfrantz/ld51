#ifndef LD39_BCD_H
#define LD39_BCD_H
#include <stdint.h>
#include "util/compiler.h"

uint16_t fastcall bcd_add16(uint16_t a, uint16_t b);
uint16_t fastcall bcd_sub16(uint16_t a, uint16_t b);

#endif // LD39_BCD_H
