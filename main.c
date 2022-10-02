#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "levels/levels.h"
#include "mappers/mmc3/mmc3.h"
#include "neslib/globals.h"
#include "neslib/ppu/ppu.h"
#include "neslib/controller/controller.h"
#include "neslib/util/random.h"
#include "neslib/util/registers.h"
#include "game/screen.h"
#include "game/entity.h"

extern const uint8_t unscii[];
extern const uint8_t sprites[];
extern const uint8_t background[];
extern void write();
const void* const useless_decl_to_force_linking[] = {
    (const void const*)sprites,
    (const void const*)unscii,
    (const void const*)background,
    (const void const*)write,
};

const uint8_t palette[32] = {
    // Background palette
    0x22, 0x03, 0x23, 0x30,  // skyblue, dark purple, med purple, white
    0x0f, 0x11, 0x21, 0x30,  // black, blue, blue, white
    0x0f, 0x07, 0x17, 0x0f,  // black, brown, light brown, black
    0x0f, 0x0a, 0x1a, 0x0f,  // black, green, light green, black

    // Sprite palette
    0x0f,0x17,0x26,0x12,    // Player
    0x0f,0x0f,0x12,0x30,    // stuff 1
    0x0f,0x37,0x07,0x17,    // stuff 2
    0x0f,0x00,0x10,0x30,    // stuff 3
};
const char kHex[] = "0123456789ABCDEF";

uint8_t framenum;
uint8_t player_pad;
uint8_t player_pad_changed;
uint8_t spridx;

void main(void) {
    rand_seed(0);
    // MMC3 horizontal mirroring.
    register_write8(0xA000, 1);
    bank_bg(1);
    bank_spr(0);
    ppu_off();
    pal_all(level_palette);  // set palette for sprites
    oam_size(1);

    screen_regenerate(1, 1);

    ppu_on_all();
    set_split(31);
    player_init(1, 1);

    for (framenum=0;;++framenum) {
        scroll0(0, 0);
        ppu_waitnmi();
        oam_clear();
        spridx = 0;

        player_pad_changed = pad_trigger(0);
        player_pad = pad_state(0);
        entity_all();
    }
}
