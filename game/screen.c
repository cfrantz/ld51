#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "game/screen.h"
#include "levels/levels.h"

#include "neslib/ppu/ppu.h"
#include "neslib/util/random.h"
#include "util/compiler.h"

#define WIDTH 7
#define HEIGHT 12




#define NORTH 1
#define SOUTH 2
#define EAST  4
#define WEST  8

#define CELLS_LEN 128
// We make the array size 8 because its a power of 2.
uint8_t maze[12][8];
uint8_t cells[CELLS_LEN];
uint8_t ncells = 0;
uint8_t grid[HEIGHT*2+1][16];


static uint8_t rand_range(uint8_t max, uint8_t mask) {
    // Since we can't do modulus, keep re-rolling the RNG if the
    // value isn't in the desired range.
    uint8_t result;
    for(;;) {
        result = rand8() & mask;
        if (result < max) return result;
    }
}

static uint8_t opposite(uint8_t dir) {
    switch(dir) {
        case NORTH: return SOUTH;
        case SOUTH: return NORTH;
        case EAST: return WEST;
        case WEST: return EAST;
    }
}

static uint8_t nextx(uint8_t dir, uint8_t x) {
    switch(dir) {
        case NORTH: return x;
        case SOUTH: return x;
        case EAST: return x+1;
        case WEST: return x-1;
    }
}

static uint8_t nexty(uint8_t dir, uint8_t y) {
    switch(dir) {
        case NORTH: return y-1;
        case SOUTH: return y+1;
        case EAST: return y;
        case WEST: return y;
    }
}

static uint8_t nextdir(uint8_t chosen) {
    uint8_t dir;
    for(;;) {
        dir = 1 << (rand8() & 3);
        if ((dir & chosen) == 0) return dir;
    }
}

static void render(uint8_t palette) {
    uint8_t x, y, dir;
    memset(grid, palette | 16, sizeof(grid));
    for(y=0; y<HEIGHT; ++y) {
    for(x=0; x<WIDTH; ++x) {
        dir = maze[y][x];
        if (!dir) continue;
        grid[y*2+1][x*2+1] = palette| 0;
        if (dir & NORTH) grid[y*2+0][x*2+1] = palette| 0;
        if (dir & SOUTH) grid[y*2+2][x*2+1] = palette| 0;
        if (dir & EAST)  grid[y*2+1][x*2+2] = palette| 0;
        if (dir & WEST)  grid[y*2+1][x*2+0] = palette| 0;
    }
    }
}

static uint8_t cells_last(void) {
    uint8_t i = ncells;
    while(i && cells[i] == 255) {
        --i;
    }
    return i;
}

void growing_tree(void) {
    uint8_t x, y, nx, ny, index, dir, chosen;
    memset(maze, 0, sizeof(maze));

    ncells = 0;
    memset(cells, 0xff, sizeof(cells));

    x = rand_range(WIDTH, 8-1);
    y = rand_range(HEIGHT, 16-1);
    //printf("start = %d, %d\n", x, y);
    cells[++ncells] = (y<<4) | x;
    index = ncells;
    while(index) {
        y = cells[index];
        x = y & 0x0F;
        y >>= 4;
        //printf("index = %u loc=%d,%d\n", index, x, y);
        chosen = 0;
        while(chosen != 0x0F) {
            dir = nextdir(chosen);
            chosen |= dir;
            //printf("dir = %d ch = %x\n", dir, chosen);
            nx = nextx(dir, x); ny = nexty(dir, y);
            //printf("dir=%d, next=%d,%d v=%x\n", dir, nx, ny, maze[ny][nx]);
            if (nx < WIDTH && ny < HEIGHT && maze[ny][nx] == 0) {
                maze[y][x] |= dir;
                maze[ny][nx] |= opposite(dir);
                cells[++ncells] = (ny<<4) | nx;
                index = 0;
                break;
            }
        }
        //printf("del = %d\n", index);
        if (index) cells[index] = 0xFF;
        index = cells_last();
    }
}


uint8_t fastcall screen_get(uint8_t x, uint8_t y) {
    return grid[y][x];
}

void generate(void) {
    int8_t i;

    for(i=4; i>=0; --i) {
        pal_bright(i);
        ppu_waitnmi();
        ppu_waitnmi();
    }
    ppu_off();
    growing_tree();
    screen_put();
    ppu_on_all();
    for(i=0; i<5; ++i) {
        pal_bright(i);
        ppu_waitnmi();
        ppu_waitnmi();
    }
}


const uint16_t ppu_dests[] = { 0x2080, 0x2800 };
const uint16_t ppu_pal_dests[] = { 0x23c8, 0x2bc0 };
uint8_t * const screens[] = {
    &grid[0][0], &grid[13][0]
};


void screen_load_one(uint8_t scrn) {
    static uint8_t x, y, i, a, h, t;
    static uint8_t *p;

    h = scrn==0 ? 13: 12;
    p = screens[scrn];
    vram_adr(ppu_dests[scrn]);
    for(y=0;y<h; ++y) {
        for(i=0; i<2; ++i) {
            for(x=0; x<16; ++x) {
                t = y*16+x;
                a = p[t] * 4;
                vram_put(level_objset[a + i + 0]);
                vram_put(level_objset[a + i + 2]);
            }
        }
    }

    vram_adr(ppu_pal_dests[scrn]);
    for(y=0;y<h; y+=2) {
        for(x=0; x<16; x+=2) {
            t = y*16+x;
            a = ((p[t+0] & 0xC0) >> 6) |
                ((p[t+1] & 0xC0) >> 4) |
                ((p[t+16] & 0xC0) >> 2) |
                ((p[t+17] & 0xC0) >> 0);
            vram_put(a);
        }
    }
}

void screen_put(void) {
    render(1 << 6);
    screen_load_one(0);
    screen_load_one(1);
    /*
    uint8_t x, y, tile;
    vram_adr(0x2000);
    for(x=0; x<128; ++x) {
        vram_put(0);
    }
    for(y=0; y<HEIGHT*2+1; ++y) {
        if (y == 13) {
            for(x=0; x<64; ++x) {
                vram_put(0);
            }
            vram_adr(0x2800);
        }
        for(x=0; x<32; ++x) {
            tile = grid[y][x & 15];
            if (tile == 0) {
                vram_put(0x20);
                vram_put(0x20);
            } else {
                vram_put(0x84);
                vram_put(0x84);
            }
        }
    }
    */
}

/*
void screen_load(uint8_t id) {
    static uint8_t n, i;
    set_mmc3_low_bank(id >> 3);
    n = id & 7;
    pal_all(levels0[n].palette);
    screen_copy_to_ram(n);
    for(i=0; i<4; i++) {
        screen_load_one(i);
    }
    for(i=0; i<6; i++) {
        mmc3_reg(i, levels0[n].chrbanks[i]);
    }
}
*/
