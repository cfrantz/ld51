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
uint8_t x, y;

const char kHex[] = "0123456789ABCDEF";

const char kScreen1[] = "11111111111111111111111111111111";
const char kScreen2[] = "22222222222222222222222222222222";
const char kScreen3[] = "33333333333333333333333333333333";
const char kScreen4[] = "44444444444444444444444444444444";
const char kHud0[] =    "+------------------------------+";
const char kHud1[] =    "|                              |";

void dobrk(void) {
    asm("brk");
    asm("nop");
}

void main(void)
{
    bank_bg(0);
    bank_spr(0);
    ppu_off();
    pal_all(palette); //set palette for sprites
    oam_size(0);

    for(val=0; val<30; ++val) {
        vram_puts(0, (32*0)+val, kScreen1);
        vram_puts(0, (32*1)+val, kScreen2);
        vram_puts(0, (32*2)+val, kScreen3);
        vram_puts(0, (32*3)+val, kScreen4);
//        vram_puts(32, val, kScreen2);
//        vram_puts(0, val+30, kScreen3);
//        vram_puts(32, val+30, kScreen4);
    }

    vram_puts(0, 32*2+25, kHud0);
    vram_puts(0, 32*2+26, kHud1);
    vram_puts(0, 32*2+27, kHud1);
    vram_puts(0, 32*2+28, kHud1);
    vram_puts(0, 32*2+29, kHud0);
    vram_puts(5, 32*2+27, "Health & Stuff");

    ppu_on_all();
    set_split(25*8);

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
        if (player_pad & PAD_LEFT) --x;
        if (player_pad & PAD_RIGHT) ++x;
        if (player_pad & PAD_UP) --y;
        if (player_pad & PAD_DOWN) ++y;

        scroll0(x, y);
        scroll1(0, 240+25*8);
    }
}
