#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef MAPPER_MMC3
#include "mappers/mmc3/mmc3.h"
#endif
#ifdef MAPPER_MMC5
#include "mappers/mmc5/mmc5.h"
#endif

#include "neslib/globals.h"
#include "neslib/ppu/ppu.h"
#include "neslib/controller/controller.h"

extern const uint8_t unscii[];
const void* const useless_decl_to_force_linking[] = {
    (const void const*)unscii,
};

const uint8_t palette[32] = {
    // Background palette
    0x22, 0x03, 0x23, 0x30,  // skyblue, dark purple, med purple, white
    0x0f, 0x11, 0x21, 0x30,  // black, blue, blue, white
    0x0f, 0x07, 0x17, 0x0f,  // black, brown, light brown, black
    0x0f, 0x0a, 0x1a, 0x0f,  // black, green, light green, black

    // Sprite palette
    0x0F, 0x02, 0x07, 0x26,  // xx, blue, brown, pink
    0x0F, 0x16, 0x2d, 0x0f,  // xx, light red, gray, black
    0x0F, 0x02, 0x11, 0x20,  // xx, blue, light blue, white
    0x0F, 0x2c, 0x28, 0x30,  // xx, cyan, yellow, white
};

const char kBars[] = "||||||||||||||||||||||||||||||||";
const char kHex[] = "0123456789ABCDEF";

uint8_t spridx;
uint8_t val;

uint8_t x, y, color;

void main(void) {
#ifdef MAPPER_MMC3
    // CHR bank 0
    mmc3_reg(0, 0);
    mmc3_reg(1, 2);
    // CHR bank 1
    mmc3_reg(2, 0);
    mmc3_reg(3, 1);
    mmc3_reg(4, 2);
    mmc3_reg(5, 3);
#endif

    bank_bg(0);
    bank_spr(1);
    ppu_off();
    pal_all(palette);  // set palette for sprites
    oam_size(0);

    vram_puts(10, 3, "Hello World!");
    vram_puts(0, 12, kBars);
    vram_puts(0, 13, kBars);
    vram_puts(9, 20, "Goodbye World!");

    vram_puts(10, 32 + 4, "Hello Screen2");
    vram_puts(0, 32 + 12, kBars);
    vram_puts(0, 32 + 13, kBars);
    vram_puts(9, 32 + 21, "Goodbye Screen2");

    for (val = 0; val < 32; ++val) {
        vram_adr(0x2000 + val * 32);
        vram_put(kHex[val >> 4]);
        vram_put(kHex[val & 15]);
    }

    ppu_on_all();

    set_split(13 * 8 - 1);
    for (;;) {
        ppu_waitnmi();
        oam_clear();
        val = pad_trigger(0);
        if (val & PAD_A) { color += 1; }
        if (val & PAD_B) { color -= 1; }

        val = pad_state(0);
        if (val & PAD_UP) { y -= 1; }
        if (val & PAD_DOWN) { y += 1; }
        if (val & PAD_LEFT) { x -= 1; }
        if (val & PAD_RIGHT) { x += 1; }

        val = frame_count;
        spridx = 0;
        spridx = oam_spr(x+8, y+32, kHex[val >> 4], color & 3, spridx);
        spridx = oam_spr(x+16, y+32, kHex[val & 15], color & 3, spridx);

        val = frame_count >> 8;
        spridx = oam_spr(x+8, y+40, kHex[val >> 4], color & 3, spridx);
        spridx = oam_spr(x+16, y+40, kHex[val & 15], color & 3, spridx);

        scroll0(frame_count, 0);
        scroll1(-frame_count, 13 * 8);
    }
}
