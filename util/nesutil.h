#ifndef NES_DEMOS_NESUTIL_H
#define NES_DEMOS_NESUTIL_H

#include "neslib.h"

// Some helpful macros
#define false       0
#define true        1
#define VADDR(x,y)	((0x2000|((y)<<5)|x))
#define MSB(x)		(((x)>>8))
#define LSB(x)		(((x)&0xff))

#define writereg8(reg_, val_)  ( *(uint8_t*)(reg_) = val_ )
#define readreg8(reg_)         ( *(uint8_t*)(reg_) )
#define writereg16(reg_, val_) ( *(uint16_t*)(reg_) = val_ )
#define readreg16(reg_)        ( *(uint16_t*)(reg_) )

#define set_mmc3_low_bank(b_) (\
        writereg8(0x8000, 6), \
        writereg8(0x8001, b_) )


#define dbg_str(val_) xputs(val_)

#define dbg_hex8(val_) ( \
        xputc(xdigits[(val_ >> 4) & 15]), \
        xputc(xdigits[(val_ >> 0) & 15]) )

#define dbg_hex16(val_) ( \
        xputc(xdigits[(val_ >> 12) & 15]), \
        xputc(xdigits[(val_ >> 8) & 15]), \
        xputc(xdigits[(val_ >> 4) & 15]), \
        xputc(xdigits[(val_ >> 0) & 15]) )

#define dbg_hex32(val_) ( \
        xputc(xdigits[(val_ >> 28) & 15]), \
        xputc(xdigits[(val_ >> 24) & 15]), \
        xputc(xdigits[(val_ >> 20) & 15]), \
        xputc(xdigits[(val_ >> 16) & 15]), \
        xputc(xdigits[(val_ >> 12) & 15]), \
        xputc(xdigits[(val_ >> 8) & 15]), \
        xputc(xdigits[(val_ >> 4) & 15]), \
        xputc(xdigits[(val_ >> 0) & 15]) )

#define dbg_hexout(val_) writereg8(0x401a, val_)


#ifndef PER_FRAME_PPU_UPDATE_LENGTH
#define PER_FRAME_PPU_UPDATE_LENGTH 10
#endif
extern uint8_t update_list[];
extern uint8_t update_count;
extern const char xdigits[];

//////////////////////////////////////////////////////////////////////
// I hacked fceux to put whatever is written to memory location
// 0x4018 to stdout.  Hooray, now I can have printf style debugging
// in my program.
//////////////////////////////////////////////////////////////////////
#define xputc(ch) ( *(char*)0x4018 = ch )
#define measure() ( *(char*)0x4019 )

//////////////////////////////////////////////////////////////////////
// Put an ascii-encoded string into VRAM
//////////////////////////////////////////////////////////////////////
extern void __fastcall__
vram_puts(unsigned char x, unsigned char y, const char *str);

extern void __fastcall__
update(unsigned char x, unsigned char y, unsigned char val);

#define update_reset() (update_count = 0)

#define PPU_CTRL	 0x2000
#define PPU_MASK	 0x2001
#define PPU_STATUS	 0x2002
#define PPU_OAM_ADDR 0x2003
#define PPU_OAM_DATA 0x2004
#define PPU_SCROLL	 0x2005
#define PPU_ADDR	 0x2006
#define PPU_DATA	 0x2007

#define vram_put_addr(a_) ( \
        writereg8(PPU_ADDR, (a_)>>8), \
        writereg8(PPU_ADDR, (a_)) )

#define vram_put_data(d_) writereg8(PPU_DATA, d_)


extern void __fastcall__
xputs(const char *str);

void __fastcall__ my_memcpy(void *dst, void *src, int len);

#endif // NES_DEMOS_NESUTIL_H
