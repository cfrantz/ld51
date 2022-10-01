#ifndef EMPTYNES_NESLIB_PPU_PPU_H_
#define EMPTYNES_NESLIB_PPU_PPU_H_
#include <stdint.h>
#include "util/compiler.h"

#include "neslib/ppu/oam.h"
#include "neslib/ppu/palette.h"
#include "neslib/ppu/scroll.h"

#define VADDR(x,y)	((0x2000|((y)<<5)|x))

void fastcall ppu_off(void);
void fastcall ppu_on_all(void);
void fastcall ppu_on_bg(void);
void fastcall ppu_on_spr(void);
void fastcall ppu_mask(uint8_t mask);
void fastcall ppu_waitnmi(void);
void fastcall bank_spr(uint8_t n);
void fastcall bank_bg(uint8_t n);
void fastcall vram_read(uint8_t *dst, uint16_t adr, uint16_t size);
void fastcall vram_write(uint8_t *src, uint16_t adr, uint16_t size);
void fastcall ppu_macro_byte(uint8_t b);
void fastcall ppu_macro_word(uint16_t b);
void fastcall vram_adr(uint16_t adr);
void fastcall vram_put(uint8_t n);
void fastcall vram_fill(uint8_t n, uint16_t len);
void fastcall vram_inc(uint8_t n);

void fastcall vram_puts(uint8_t x, uint8_t y, const char *str);

#endif  // EMPTYNES_NESLIB_PPU_PPU_H_
