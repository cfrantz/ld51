#ifndef EMPTYNES_NESLIB_PPU_PALETTE_H_
#define EMPTYNES_NESLIB_PPU_PALETTE_H_

#include <stdint.h>

void fastcall pal_all(const char *data);
void fastcall pal_bg(const char *data);
void fastcall pal_spr(const char *data);
void fastcall pal_col(uint8_t index, uint8_t color);
void fastcall pal_clear(void);
void fastcall pal_bright(uint8_t bright);
#endif  // EMPTYNES_NESLIB_PPU_PALETTE_H_
