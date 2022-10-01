#include <stdint.h>

#include "neslib/ppu/ppu.h"

void fastcall vram_puts(uint8_t x, uint8_t y, const char *str)
{
	char ch;
	vram_adr(VADDR(x, y));
	while((ch=*str++) != 0) {
		vram_put(ch);
	}
}
