#include <stdint.h>
#include "nesutil.h"

const char xdigits[] = "0123456789abcdef";

uint8_t update_list[PER_FRAME_PPU_UPDATE_LENGTH * 3];
uint8_t update_count;

// Supply a 'write' function so cc65's printf goes to my debug port
void write(int fd, const char *buf, int len)
{
	while(len--) {
		xputc(*buf++);
	}
}

void __fastcall__ xputs(const char *str) {
    while(*str) {
        xputc(*str++);
    }
}

void __fastcall__ update(unsigned char x, unsigned char y, unsigned char val) {
    update_list[update_count] = MSB(VADDR(x, y));
    update_list[++update_count] = LSB(VADDR(x, y));
    update_list[++update_count] = val;
    ++update_count;
}

//////////////////////////////////////////////////////////////////////
// Put an ascii-encoded string into VRAM
//////////////////////////////////////////////////////////////////////
void __fastcall__ vram_puts(unsigned char x, unsigned char y, const char *str)
{
	char ch;
	vram_adr(VADDR(x, y));
	while((ch=*str++) != 0) {
		vram_put(ch);
	}
}

void __fastcall__ my_memcpy(void *dst, void *src, int len) {
    uint8_t *d = (uint8_t*)dst;
    uint8_t *s = (uint8_t*)src;
    while(len) {
        *d = *s;
        ++d; ++s; --len;
    }
}
