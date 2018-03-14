#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "nesutil.h"
#include "levels.h"
#include "screen.h"
#include "entity.h"

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
uint8_t load_bank;
uint8_t player_pad;
uint8_t player_pad_changed;
uint8_t spridx;

enum GameMode {
    TITLE_SCREEN,
    GAME,
    PAUSE,
    LOAD_NEXT,
};

enum GameMode game_state;

void main(void)
{
    static uint8_t state;
    bank_bg(0);
    bank_spr(1);
    ppu_off();
    pal_all(palette); //set palette for sprites
    oam_size(1);
    ppu_on_all();
    //entity_load_screen();
    for(framenum=0;;++framenum) {
        //wait for next TV frame
        writereg8(0x4019, 0);
        ppu_waitnmi();
        writereg8(0x4019, 1);
        oam_clear();
        player_pad_changed = pad_trigger(0);
        player_pad = pad_state(0);

        switch(game_state) {
            case TITLE_SCREEN:
                scroll(framenum, 32);
                if (player_pad_changed & PAD_START) {
                    load_bank = header0.next_bank;
                    game_state = LOAD_NEXT;
                }
                break;
            case LOAD_NEXT:
                ppu_off();
                set_mmc3_low_bank(load_bank);
                entity_kill_all();
                copy_to_vram_simple(0, 0);
                copy_to_vram_simple(1, 1);
                entity_spawn_screen(0);
                entity_taken_reset();
                entity_set_player(header0.start_px, header0.start_py, true);
                ppu_on_all();
                game_state = GAME;
                break;
            case GAME:
                entity_newframe();
                state = entity_player_control();
                if (state == PLAYER_DEAD) {
                    entity_player_checkpoint();
                    break;
                } else if (state == PLAYER_DONE) {
                    if (!entity_player_addpoints()) {
                        load_bank = header0.next_bank;
                        game_state = LOAD_NEXT;
                        break;
                    }
                } else {
                    entity_update_all();
                    entity_compute_position(0);
                }
                entity_draw(0);
                entity_draw_all();
                if (player_pad_changed & PAD_START)
                    game_state = PAUSE;

                break;
            case PAUSE:
                pause();
                entity_draw(0);
                entity_draw_all();
                if (player_pad_changed & PAD_START)
                    game_state = GAME;
                break;
            default:
                game_state = GAME;
        }
    }
}
