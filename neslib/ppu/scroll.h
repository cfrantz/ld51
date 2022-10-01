#ifndef EMPTYNES_NESLIB_PPU_SCROLL_H_
#define EMPTYNES_NESLIB_PPU_SCROLL_H_
#include <stdint.h>

#include "util/compiler.h"

/*
 * set_split should be provided by the mapper implementation.
 */
void fastcall set_split(uint8_t ypos);
void fastcall scroll0(uint16_t x, uint16_t y);
void fastcall scroll1(uint16_t x, uint16_t y);
#endif  // EMPTYNES_NESLIB_PPU_SCROLL_H_
