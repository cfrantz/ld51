#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "util/nesutil.h"

const unsigned char palette[32]={
    // Background palette
    0x22, 0x03, 0x23, 0x30, // skyblue, dark purple, med purple, white
    0x0f, 0x11, 0x21, 0x30, // black, blue, blue, white
    0x0f, 0x07, 0x17, 0x0f, // black, brown, light brown, black
    0x0f, 0x0a, 0x1a, 0x0f, // black, green, light green, black

    // Sprite palette
    0x0F, 0x02, 0x07, 0x26, // xx, blue, brown, pink
    0x0F, 0x16, 0x2d, 0x0f, // xx, light red, gray, black
    0x0F, 0x02, 0x11, 0x20, // xx, blue, light blue, white
    0x0F, 0x2c, 0x28, 0x30, // xx, cyan, yellow, white
};

uint16_t framenum;
uint8_t player_pad;
uint8_t player_pad_changed;
uint8_t pad;
uint8_t val;
uint8_t spridx;

const char kBars[] = "||||||||||||||||||||||||||||||||";
const char kCount0[] = "0         1         2         3 ";
const char kCount1[] = "01234567890123456789012345678901";
const char kHex[] = "0123456789ABCDEF";

void dobrk(void) {
    asm("brk");
    asm("nop");
}

void xram_puts(uint8_t x, uint8_t y, const char* ptr) {
    char* addr = (char*)0x5c00;
    addr += x;
    addr += (uint16_t)y << 5;
    while(*ptr) {
        *addr++ = *ptr++;
    }
}

uint8_t xs, ys;
uint8_t state;

void main(void)
{
    bank_bg(0);
    bank_spr(0);
    ppu_off();
    pal_all(palette); //set palette for sprites
    oam_size(0);

    // Arrange nametables as:
    // Vram0 Vram1
    // Fill Fill
    writereg8(0x5105, 0xf4);

    // Set extended RAM as extra nametable.
    writereg8(0x5104, 0);

    // Fill table as 'F' in palette 3.
    writereg8(0x5106, 'F');
    writereg8(0x5107, 3);

    // Vertical split vscroll = 0, bank = 0
    writereg8(0x5201, 0);
    writereg8(0x5202, 0);

    vram_puts(10, 3, "Hello World!");
    vram_puts(0, 12, kBars);
    vram_puts(0, 13, kBars);
    vram_puts(9, 20, "Goodbye World!");

    vram_puts(10,32+4, "Hello Screen2");
    vram_puts(0, 32+12, kBars);
    vram_puts(0, 32+13, kBars);
    vram_puts(9, 32+21, "Goodbye Screen2");
    for(val=0; val<32; ++val) {
        vram_adr(0x2000 + val*32);
        vram_put(kHex[val>>4]);
        vram_put(kHex[val&15]);
    }

    xram_puts(0, 1, kCount0);
    xram_puts(0, 2, kCount1);
    xram_puts(5, 3, "Hello ExtRam!");
    xram_puts(24,4, "Goodbye!");
    xram_puts(0, 27, kCount0);
    xram_puts(0, 28, kCount1);

    set_split(0);
    ppu_on_all();

    for(framenum=0;;++framenum) {
        ppu_waitnmi();
        oam_clear();
        spridx = 0;
        val = readreg8(0);
        spridx = oam_spr(8, 32, kHex[val>>4], 3, spridx);
        spridx = oam_spr(16, 32, kHex[val&15], 3, spridx);

        val = readreg8(1);
        spridx = oam_spr(8, 40, kHex[val>>4], 3, spridx);
        spridx = oam_spr(16, 40, kHex[val&15], 3, spridx);

        player_pad_changed = pad_trigger(0);
        player_pad = pad_state(0);
        if (player_pad & PAD_UP) {
            if (ys > 0) --ys;
        } else if (player_pad & PAD_DOWN) {
            if (ys < 240) ++ys;
        }
        if (player_pad & PAD_LEFT) {
            if (xs > 0) --xs;
        } else if (player_pad & PAD_RIGHT) {
            if (xs < 255) ++xs;
        }

        if (player_pad_changed & PAD_A) {
            state ^= 1;
            if (state & 1) {
                writereg8(0x5200, 0xC0 + 24);
            } else {
                writereg8(0x5200, 0);
            }
        }
        if (player_pad_changed & PAD_B) {
            state ^= 2;
            if (state & 2) {
                writereg8(0x5200, 0x80 + 24);
            } else {
                writereg8(0x5200, 0);
            }
        }

        scroll0(xs, ys);
    }
}
