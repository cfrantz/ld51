#ifndef EMPTYNES_NESLIB_PPU_OAM_H
#define EMPTYNES_NESLIB_PPU_OAM_H
#include <stdint.h>

#include "util/compiler.h"

#define OAM_FLIP_V		0x80
#define OAM_FLIP_H		0x40
#define OAM_BEHIND		0x20

void fastcall oam_clear(void);
void fastcall oam_size(uint8_t size);
uint8_t fastcall oam_spr(uint8_t x, uint8_t y, uint8_t chrnum, uint8_t attr,
                         uint8_t sprid);
uint8_t fastcall oam_meta_spr(uint8_t x, uint8_t y, uint8_t sprid,
                              const uint8_t *data);
void fastcall oam_hide_rest(uint8_t sprid);
#endif  // EMPTYNES_NESLIB_PPU_OAM_H
