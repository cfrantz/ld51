#include <stdint.h>

#include "neslib/ppu/ppu.h"
#include "levels/levels.h"
#include "music/cfplayer.h"

void transition(void) {
    uint8_t i;
    
    cfplayer_now_playing = 0;
    cfplayer_reset_song();

    ppu_off();
    scroll1(0, 32);
    vram_adr(0x2040);
    vram_fill(0, 1024-64);
    vram_write(level.name, 0x2000 + 14*32, 32);
    oam_clear();
    pal_bright(0);
    ppu_on_all();

    for(i=0; i<20; ++i) {
        if (i%4 == 0) pal_bright(i/4);
        ppu_waitnmi();
    }
    for(i=0; i<60*3; ++i) {
        ppu_waitnmi();
    }
    // The entity code will fade us in.
    pal_bright(0);
}
