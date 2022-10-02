#include <stdio.h>
#include <stdint.h>
#include "game/entity.h"
#include "neslib/controller/controller.h"
#include "neslib/ppu/ppu.h"
#include "neslib/util/random.h"

#include "game/screen.h"


const uint8_t meta_sprites[] = {
    // player left 1
    0x00, 0x00, 0x1a, 0x40,
    0x08, 0x00, 0x18, 0x40, 0x80,
    // player left 2
    0x00, 0x00, 0x1e, 0x40,
    0x08, 0x00, 0x1c, 0x40, 0x80,
    // player right 1
    0x00, 0x00, 0x18, 0x00,
    0x08, 0x00, 0x1a, 0x00, 0x80,
    // player right 2
    0x00, 0x00, 0x1c, 0x00,
    0x08, 0x00, 0x1e, 0x00, 0x80,
    // player up 1
    0x00, 0x00, 0x14, 0x00,
    0x08, 0x00, 0x16, 0x00, 0x80,
    // player up 2
    0x00, 0x00, 0x16, 0x40,
    0x08, 0x00, 0x14, 0x40, 0x80,
    // player down 1
    0x00, 0x00, 0x10, 0x00,
    0x08, 0x00, 0x12, 0x00, 0x80,
    // player down 2
    0x00, 0x00, 0x12, 0x40,
    0x08, 0x00, 0x10, 0x40, 0x80,
    // spider 1
    0x00, 0x00, 0x80, 0x01,
    0x08, 0x00, 0x82, 0x01, 0x80,
    // spider 2
    0x00, 0x00, 0x82, 0x41,
    0x08, 0x00, 0x80, 0x41, 0x80,
    // grump 1
    0x00, 0x00, 0x84, 0x01,
    0x08, 0x00, 0x86, 0x01, 0x80,
    // grump 2
    0x00, 0x00, 0x86, 0x41,
    0x08, 0x00, 0x84, 0x41, 0x80,
    // skull 1
    0x00, 0x00, 0x88, 0x01,
    0x08, 0x00, 0x8a, 0x01, 0x80,
    // skull 2
    0x00, 0x00, 0x8a, 0x41,
    0x08, 0x00, 0x88, 0x41, 0x80,

    // star 1
    0x00, 0x00, 0x70, 0x01,
    0x08, 0x00, 0x72, 0x01, 0x80,
    // star 2
    0x00, 0x00, 0x74, 0x01,
    0x08, 0x00, 0x76, 0x01, 0x80,
    // star 3
    0x00, 0x00, 0x78, 0x01,
    0x08, 0x00, 0x7a, 0x01, 0x80,
    // star 4
    0x00, 0x00, 0x7c, 0x01,
    0x08, 0x00, 0x7e, 0x01, 0x80,

};
const uint8_t item_meta_sprites[] = {
    // sword left
    0x00, 0x00, 0x24, 0x01,
    0x08, 0x00, 0x26, 0x01, 0x80,
    // sword right
    0x00, 0x00, 0x26, 0x41,
    0x08, 0x00, 0x24, 0x41, 0x80,
    // sword up
    0x00, 0x00, 0x20, 0x01, 0x80,
    // sword down
    0x00, 0x00, 0x20, 0x81, 0x80,
    // 6502
    0x00, 0x00, 0x30, 0x01,
    0x08, 0x00, 0x32, 0x01, 0x80,
    // PPU
    0x00, 0x00, 0x34, 0x01,
    0x08, 0x00, 0x36, 0x01, 0x80,
    // RAM
    0x00, 0x00, 0x38, 0x01,
    0x08, 0x00, 0x3a, 0x01, 0x80,
    // 7404
    0x00, 0x00, 0x3c, 0x01,
    0x08, 0x00, 0x3e, 0x01, 0x80,
};

const uint8_t * const ids[] = {
    &meta_sprites[0*9],
    &meta_sprites[1*9],
    &meta_sprites[2*9],
    &meta_sprites[3*9],
    &meta_sprites[4*9],
    &meta_sprites[5*9],
    &meta_sprites[6*9],
    &meta_sprites[7*9],

#define SPR_SPIDER 8
    &meta_sprites[8*9],
    &meta_sprites[9*9],
#define SPR_GRUMP 10
    &meta_sprites[10*9],
    &meta_sprites[11*9],
#define SPR_SKULL 12
    &meta_sprites[12*9],
    &meta_sprites[13*9],
#define SPR_STAR 14
    &meta_sprites[14*9],
    &meta_sprites[15*9],
    &meta_sprites[16*9],
    &meta_sprites[17*9],
#define SPR_SWORD 18
    &item_meta_sprites[0],
    &item_meta_sprites[9],
    &item_meta_sprites[18],
    &item_meta_sprites[23],
#define SPR_6502 22
    &item_meta_sprites[28],
#define SPR_PPU 23
    &item_meta_sprites[28+9],
#define SPR_RAM 24
    &item_meta_sprites[28+9+9],
#define SPR_7404 25
    &item_meta_sprites[28+9+9+9],
};

enum EntityID {
    Player = 0,
    Spider = 1,
    Grump = 2,
    Skull = 3,
    Sword = 8,
    CPU = 9,
    PPU = 10,
    RAM = 11,
    Inverter = 12,
};
#define ITEM_IDX(x_) (x_&7)

enum EntityState {
    Normal = 0,
    Stun = 1,
    Dead = 2,
};

const uint8_t item_sprite[] = {
    SPR_SWORD+2,
    SPR_6502,
    SPR_PPU,
    SPR_RAM,
    SPR_7404,
};

//                    player enemies                     items
const uint8_t bbx0[] = { 3,  2,  2,  2,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0, };
const uint8_t bby0[] = { 4,  2,  2,  2,  0,  0,  0,  0,  0,  2,  2,  2,  2,  0,  0,  0, };
const uint8_t bbx1[] = { 12, 14, 14, 14, 0,  0,  0,  0,  5,  14,14, 14, 14,  0,  0,  0, };
const uint8_t bby1[] = { 14, 14, 14, 14, 0,  0,  0,  0, 12,  12,12, 12, 12,  0,  0,  0, };


const uint8_t health[]={ 8,  4,  9,  16,  0,  0,  0,  };

uint8_t curmap;
#define NUM_ENTITIES 16
uint8_t entity_id[NUM_ENTITIES];
int8_t entity_hp[NUM_ENTITIES];
uint8_t entity_facing[NUM_ENTITIES];
uint8_t entity_timer[NUM_ENTITIES];
uint8_t entity_dmgtm[NUM_ENTITIES];
uint8_t entity_sprite[NUM_ENTITIES];
uint8_t entity_state[NUM_ENTITIES];
uint8_t entity_subpixx[NUM_ENTITIES];
uint8_t entity_subpixy[NUM_ENTITIES];
uint16_t entity_posx[NUM_ENTITIES];
uint16_t entity_posy[NUM_ENTITIES];
int16_t entity_vx[NUM_ENTITIES];
int16_t entity_vy[NUM_ENTITIES];
uint16_t scrollx, scrolly;
uint8_t screenx, screeny;
uint8_t tmp;
uint8_t index;

const uint8_t bits[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
/*
#pragma bss-name(push, "WRAM")
uint8_t item_taken[256];
#pragma bss-name(pop)
*/
uint8_t player_has_item[8];
uint16_t player_timer;

const uint8_t sword_frames[] = {0, 12, 12, 12, 10, 8, 6, 4, 2};
uint16_t swordx, swordy;
uint8_t sword_timer;
uint8_t sword_damage;
uint8_t ssx, ssy;
const uint8_t sbx0[] = { 0,  3,  1,  1 };
const uint8_t sby0[] = { 1,  1,  0,  3 };
const uint8_t sbx1[] = { 12, 15, 5,  5 };
const uint8_t sby1[] = { 5,  5,  12, 15};

uint16_t xx, yy;
int16_t sx, sy;
uint8_t on_screen;

void entity_move(void) {
    static uint8_t id, a, b;
    static int16_t vx, vy;

    id = entity_id[index];
    xx = entity_posx[index];
    yy = entity_posy[index];
    vx = entity_vx[index];
    vy = entity_vy[index];

    if (vx < 0) {
        // left
        sx = entity_subpixx[index] + vx;
        xx += (sx >> 8);
        a = screen_get((xx+bbx0[id])/ 16, (yy+bby0[id]) / 16);
        b = screen_get((xx+bbx0[id])/ 16, (yy+bby1[id]) / 16);
    } else if (vx > 0) {
        // right
        sx = entity_subpixx[index] + vx;
        xx += (sx >> 8);
        a = screen_get((xx+bbx1[id])/ 16, (yy+bby0[id]) / 16);
        b = screen_get((xx+bbx1[id])/ 16, (yy+bby1[id]) / 16);
    }
    if (vy < 0) {
        // up
        sy = entity_subpixy[index] + vy;
        yy += (sy >> 8);
        a = screen_get((xx+bbx0[id])/ 16, (yy+bby0[id]) / 16);
        b = screen_get((xx+bbx1[id])/ 16, (yy+bby0[id]) / 16);
    } else if (vy > 0) {
        // down
        sy = entity_subpixy[index] + vy;
        yy += (sy >> 8);
        a = screen_get((xx+bbx0[id])/ 16, (yy+bby1[id]) / 16);
        b = screen_get((xx+bbx1[id])/ 16, (yy+bby1[id]) / 16);
    }
    if ((a|b) & 16) return;

    entity_subpixx[index] = sx;
    entity_posx[index] = xx;
    entity_subpixy[index] = sy;
    entity_posy[index] = yy;
}


void player_input(void) {
    entity_vx[0] = 0;
    entity_vy[0] = 0;
    if (sword_timer) {
    } else if (player_pad_changed & PAD_A && player_has_item[ITEM_IDX(Sword)]) {
        sword_timer = sizeof(sword_frames) - 1;
    } else if (player_pad & PAD_LEFT) {
        entity_facing[0] = 0;
        entity_vx[0] = -0x200;
    } else if (player_pad & PAD_RIGHT) { 
        entity_facing[0] = 1;
        entity_vx[0] = 0x200;
    } else if (player_pad & PAD_UP) {
        entity_facing[0] = 2;
        entity_vy[0] = -0x200;
    } else if (player_pad & PAD_DOWN) { 
        entity_facing[0] = 3;
        entity_vy[0] = 0x200;
    }
}    

void player_sword(void) {
    if (!sword_timer)
        return;
    swordx = entity_posx[0];
    swordy = entity_posy[0];
    if (entity_facing[0] == 0) {
        // left
        swordx -= sword_frames[sword_timer];
        swordy += 7;
    } else if (entity_facing[0] == 1) {
        // right
        swordx += sword_frames[sword_timer];
        swordy += 7;
    } else if (entity_facing[0] == 2) {
        // up
        swordx += 1;
        swordy -= sword_frames[sword_timer];
    } else if (entity_facing[0] == 3) {
        //down
        swordx += 9;
        swordy += sword_frames[sword_timer];
    }
    ssx = swordx-scrollx;
    ssy = swordy-scrolly;
    spridx = oam_meta_spr(ssx, ssy, spridx, ids[SPR_SWORD+entity_facing[0]]);
    sword_timer -= 1;
}

void player_display(void) {
    scrollx = 0;
    if (entity_posy[0] < 104) {
        scrolly = 0;
    } else if (entity_posy[0] > 311) {
        scrolly = 207;
    } else {
        scrolly = entity_posy[0] - 104;
    }
    scroll1(scrollx, scrolly+32);

    screenx = entity_posx[0] - scrollx;
    screeny = 30 + entity_posy[0] - scrolly;

    tmp = (((uint8_t)entity_posx[0] + (uint8_t)entity_posy[0]) & 4) >>2;
    tmp += entity_facing[0]*2;
    if (entity_state[0] == Stun && entity_timer[0] & 1)
        return;
    spridx = oam_meta_spr(screenx, screeny, spridx, ids[tmp]);
    player_sword();
    /*
    if (player_has_item[ITEM_IDX(Sword)]) {
        spridx = oam_meta_spr(256-23, 241-32, spridx, ids[SPR_SWORD+2]);
    }
    for(tmp=0; tmp<4; ++tmp) {
        if (player_has_item[ITEM_IDX(CPU+tmp)]) {
            spridx = oam_meta_spr(16+tmp*16, 241-32, spridx,
                    ids[item_sprite[ITEM_IDX(CPU+tmp)]]);
        }
    }
    */
}   

void entity_display(void) {
    on_screen = 0;
    sx = entity_posx[index] - scrollx;
    if (sx < 0  || sx > 255) return;
    sy = entity_posy[index] - scrolly;
    if (sy < 0  || sy > 192) return;
    on_screen = 1;
    if (entity_state[index] == Stun && entity_timer[index] & 1)
        return;
    spridx = oam_meta_spr(sx, sy, spridx, ids[entity_sprite[index]]);
}


void entity_spawn(uint8_t *spawns) {
    tmp = 0;
    for(index=1; index<9; ++index,tmp+=3) {
        entity_posx[index] = spawns[tmp+0] * 16;
        entity_posy[index] = spawns[tmp+1] * 16;
        entity_id[index] = spawns[tmp+2];
        entity_hp[index] = health[entity_id[index]];
        entity_subpixx[index] = 0;
        entity_subpixy[index] = 0;
        entity_vx[index] = 0;
        entity_vy[index] = 0;
        entity_timer[index] = 0;
        entity_dmgtm[index] = 0;
        entity_sprite[index] = 0;
        entity_state[index] = Normal;
    }
}

void item_spawn(uint8_t *spawns) {
    /*
    static uint8_t j;
    tmp = j = 0;
    for(index=15; index>8; --index,tmp+=3,++j) {
        if (item_taken[curmap] & bits[j])
            continue;
        entity_posx[index] = spawns[tmp+0] * 16;
        entity_posy[index] = spawns[tmp+1] * 16;
        entity_id[index] = spawns[tmp+2];
        entity_hp[index] = 0;
        entity_subpixx[index] = 0;
        entity_subpixy[index] = 0;
        entity_vx[index] = 0;
        entity_vy[index] = 0;
        entity_timer[index] = 0;
        entity_dmgtm[index] = 0;
        entity_sprite[index] = item_sprite[ITEM_IDX(entity_id[index])];
        entity_state[index] = Normal;
    }
    */
}

void entity_load_screen(uint8_t dmap) {
    curmap = dmap;
    //screen_load(dmap);
    index = 1;
    //entity_spawn(screen_get_spawns(dmap));
    //item_spawn(screen_get_items(dmap));
    player_display();
}

uint8_t player_check_exit(void) {
    /*
    static uint8_t i, x, y;

    x = (entity_posx[0] + 8) / 16;
    y = (entity_posy[0] + 8) / 16;
    for(i=0; i<8; ++i) {
        if (x >= screen_exit_x0[i] &&
            x < screen_exit_x1[i] &&
            y >= screen_exit_y0[i] &&
            y < screen_exit_y1[i]) {
            entity_posx[0] = screen_exit_dx[i]*16;
            entity_posy[0] = screen_exit_dy[i]*16;
            if (entity_posx[0] == 0) entity_posx[0] = 9;
            if (entity_posy[0] == 0) entity_posy[0] = 9;
            ppu_off();
            entity_load_screen(screen_exit_dmap[i]);

            ppu_on_all();
            return 1;
        }
    }
    */
    return 0;
}

void player_init(uint8_t x, uint8_t y) {
    player_timer = 600;
    entity_id[0] = 0;
    entity_posx[0] = x*16;
    entity_posy[0] = y*16;
    entity_facing[0] = 3;
    sword_damage = 3;
    for(tmp=0; tmp<8; tmp++) {
        player_has_item[0] = 0;
    }
}

void __fastcall__ entity_set_velocity(int16_t v) {
    switch(entity_facing[index]) {
        case 0: entity_vx[index] = -v; entity_vy[index] = 0; break;
        case 1: entity_vx[index] =  v; entity_vy[index] = 0; break;
        case 2: entity_vx[index] = 0; entity_vy[index] = -v; break;
        case 3: entity_vx[index] = 0; entity_vy[index] =  v; break;
    }
}

void entity_damage(void) {
    if (entity_state[index] != Dead) {
        entity_state[index] = Stun;
        entity_timer[index] = 30;
        if (entity_dmgtm[index] == 0) {
            entity_set_velocity(0);
            entity_dmgtm[index] = sword_timer;
            entity_hp[index] -= sword_damage;
            if (entity_hp[index] <= 0)
                entity_state[index] = Dead;
        }
    }
}

void player_damage(void) {
    entity_state[0] = Stun;
    entity_timer[0] = 30;
}

void entity_take(void) {
    player_has_item[ITEM_IDX(entity_id[index])] += 1;
    //item_taken[curmap] |= bits[15-index];
    entity_id[index] = 0;
}

uint8_t entity_collide(void) {
    static uint8_t id, x0, y0, x1, y1;
    if (!on_screen)
        return 0;

    id = entity_id[index];
    x0 = sx + bbx0[id];
    y0 = sy + bby0[id];
    x1 = sx + bbx1[id];
    y1 = sy + bby1[id];

    if (sword_timer &&
        ssx+sbx0[entity_facing[0]] <= x1 &&
        ssx+sbx1[entity_facing[0]] >= x0 &&
        ssy+sby0[entity_facing[0]] <= y1 &&
        ssy+sby1[entity_facing[0]] >= y0) {
        return 2;
    }
    if (entity_state[0] == Stun || entity_state[index] == Stun) {
        return 0;
    }
    if (screenx+bbx0[0] <= x1 &&
        screenx+bbx1[0] >= x0 &&
        screeny+bby0[0] <= y1 &&
        screeny+bby1[0] >= y0) {
        return 1;
    }
    return 0;
}

void entity_ai(void) {
    static uint8_t timer, tick, coll;
    timer = entity_timer[index];
    tick = timer / 4;
    if (timer) {
        entity_timer[index] -= 1;
    }
    if (entity_state[index] == Dead) {
        entity_sprite[index] = SPR_STAR + tick/2;
        if (entity_timer[index] == 0) entity_id[index] = 0;
        return;
    }
    if (entity_dmgtm[index]) {
        entity_dmgtm[index] -= 1;
    }

    if (index)
        coll = entity_collide();

    switch(entity_id[index]) {
    case Player:
        if (timer == 0) {
            if (entity_state[index] == Stun) entity_state[index] = Normal;
        }
        player_input();
        break;
    case Spider:
        if (timer == 0) {
            if (entity_state[index] == Stun) entity_state[index] = Normal;
            entity_timer[index] = rand8() & 0x3f;
            entity_facing[index] = rand8() & 3;
            entity_set_velocity(0x200);
        }
        if (coll == 1) {
            player_damage();
        } else if (coll == 2) {
            entity_damage();
        }
        entity_sprite[index] = SPR_SPIDER + (tick & 1);
        break;
    case Grump:
        if (timer == 0) {
            if (entity_state[index] == Stun) entity_state[index] = Normal;
            entity_timer[index] = rand8() & 0xc;
            entity_facing[index] = rand8() & 3;
            entity_set_velocity(0xc0);
        }
        if (coll == 1) {
            player_damage();
        } else if (coll == 2) {
            entity_damage();
        }
        entity_sprite[index] = SPR_GRUMP + (tick & 1);
        break;
    case Skull:
        if (timer == 0) {
            if (entity_state[index] == Stun) entity_state[index] = Normal;
            entity_timer[index] = rand8() & 0x3c;
            tmp = rand8() & 3;
            entity_facing[index] = tmp;
            entity_vx[index] = (tmp&1) ? 0x100 : -0x100;
            entity_vy[index] = (tmp&2) ? 0x100 : -0x100;
        }
        if (coll == 1) {
            player_damage();
        } else if (coll == 2) {
            entity_damage();
        }
        entity_sprite[index] = SPR_SKULL + (tick & 1);
        break;
    case Sword:
    case CPU:
    case PPU:
    case RAM:
    case Inverter:
        if (coll == 1) entity_take();
        break;
    }
}

void entity_all(void) {
    for(index=0; index<NUM_ENTITIES; ++index) {
        if (index && !entity_id[index]) continue;
        if (index) {
            entity_display();
        } else {
            player_display();
        }
        entity_ai();
        // Items don't move.
        if ((entity_id[index] & 8) == 0) {
            entity_move();
        }
    }
    player_timer -= 1;
    if (player_timer < 6) {
        pal_bright(player_timer);
    }
    if (player_timer >= 595) {
        pal_bright(599 - player_timer);
    }
    if (player_timer == 0) {
        screen_regenerate( entity_posx[0] / 16, entity_posy[0] / 16);
        player_timer = 600;
    }
}
