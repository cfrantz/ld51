#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "levels/levels.h"
#include "neslib/globals.h"
#include "neslib/ppu/ppu.h"
#include "neslib/controller/controller.h"
#include "neslib/util/random.h"
#include "neslib/util/registers.h"
#include "game/entity.h"
#include "game/screen.h"
#include "game/transition.h"

#ifdef MAPPER_MMC3
#include "mappers/mmc3/mmc3.h"
#define set_low_bank(x) set_mmc3_low_bank(x)
#endif

#ifdef MAPPER_VRC7
#include "mappers/vrc7/vrc7.h"
#define set_low_bank(x) vrc7_reg(8, x)
#endif

extern const uint8_t unscii[];
extern const uint8_t sprites[];
extern const uint8_t background[];
extern const uint8_t level1[];
extern const uint8_t level2[];
extern const uint8_t level3[];
extern void write();
const void* const useless_decl_to_force_linking[] = {
    (const void const*)sprites,
    (const void const*)unscii,
    (const void const*)background,
    (const void const*)level1,
    (const void const*)level2,
    (const void const*)level3,
    (const void const*)write,
};

uint8_t framenum;
uint8_t player_pad;
uint8_t player_pad_changed;
uint8_t spridx;

void main(void) {
    ppu_off();
    rand_seed(0);
#ifdef MAPPER_MMC3
    // MMC3 horizontal mirroring.
    register_write8(0xA000, 1);
#endif
    set_low_bank(0);

    bank_bg(1);
    bank_spr(0);
    pal_all(level.palette);
    oam_size(1);
    set_split(31);

    transition();
    player_init_once(level.start.x, level.start.y);
    // screen_regenerate will ppu_on_all for us.
    screen_regenerate(level.start.x, level.start.y);

    for (framenum=0;;++framenum) {
        scroll0(0, 0);
        ppu_waitnmi();
        oam_clear();
        spridx = 0;

        player_pad_changed = pad_trigger(0);
        player_pad = pad_state(0);
        entity_all();
        if (player_check_exit()) {
            set_split(0);
            set_low_bank(level.next);
            transition();
            pal_all(level.palette);
            player_init(level.start.x, level.start.y);
            entity_load_screen(level.next);
            set_split(31);
        }
    }
}
