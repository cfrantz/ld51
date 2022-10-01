#ifndef EMPTYNES_NESLIB_GLOBALS_H
#define EMPTYNES_NESLIB_GLOBALS_H
#include <stdint.h>

extern uint16_t frame_count;
#pragma zpsym("frame_count");

extern uint8_t ppu_macro_len;
#pragma zpsym("ppu_macro_len");

extern uint8_t ppu_macro[64];

#endif // EMPTYNES_NESLIB_GLOBALS_H
