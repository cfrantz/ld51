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

void main(void)
{
    bank_bg(0);
    bank_spr(1);
    ppu_off();
    pal_all(palette); //set palette for sprites
    oam_size(1);

    vram_puts(10, 10, "Hello World!");

    ppu_on_all();
    for(framenum=0;;++framenum) {
        ppu_waitnmi();
        oam_clear();
        player_pad_changed = pad_trigger(0);
        player_pad = pad_state(0);
    }
}
