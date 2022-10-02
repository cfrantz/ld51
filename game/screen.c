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
uint8_t grid[HEIGHT*2+2][16];


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

static void render(void) {
    uint8_t x, y, dir;
    memset(grid, level.fill, sizeof(grid));
    for(y=0; y<HEIGHT; ++y) {
    for(x=0; x<WIDTH; ++x) {
        dir = maze[y][x];
        if (!dir) continue;
        grid[1+y*2+1][x*2+1] = level.floor;
        if (dir & NORTH) grid[1+y*2+0][x*2+1] = level.floor;
        if (dir & SOUTH) grid[1+y*2+2][x*2+1] = level.floor;
        if (dir & EAST)  grid[1+y*2+1][x*2+2] = level.floor;
        if (dir & WEST)  grid[1+y*2+1][x*2+0] = level.floor;
    }
    }

    // Forcibly place the exit
    grid[0][6] = 0xd4;
    grid[1][6] = 0xd5;
    grid[2][6] = level.floor;
    grid[0][7] = 0xc4;
    grid[1][7] = 0xc4;
    grid[2][7] = level.floor;
    grid[0][8] = 0xd4;
    grid[1][8] = 0xd5;
    grid[2][8] = level.floor;
}

static uint8_t cells_last(void) {
    uint8_t i = ncells;
    while(i && cells[i] == 255) {
        --i;
    }
    return i;
}

void growing_tree(uint8_t x, uint8_t y) {
    uint8_t nx, ny, index, dir, chosen;
    memset(maze, 0, sizeof(maze));

    ncells = 0;
    memset(cells, 0xff, sizeof(cells));

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
        if (index) cells[index] = 0xFF;
        index = cells_last();
    }
}


uint8_t fastcall screen_get(uint8_t x, uint8_t y) {
    return grid[y][x];
}


const uint16_t ppu_dests[] = { 0x2080, 0x2800 };
const uint16_t ppu_pal_dests[] = { 0x23c8, 0x2bc0 };
uint8_t * const screens[] = {
    &grid[0][0], &grid[13][0]
};


void screen_load_one(uint8_t scrn) {
    static uint8_t x, y, i, a, h, t;
    static uint8_t *p;

    h = 13;
    p = screens[scrn];
    vram_adr(ppu_dests[scrn]);
    for(y=0;y<h; ++y) {
        for(i=0; i<2; ++i) {
            for(x=0; x<16; ++x) {
                t = y*16+x;
                a = p[t] * 4;
                vram_put(level.objset[a + i + 0]);
                vram_put(level.objset[a + i + 2]);
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

void screen_regenerate(uint8_t x, uint8_t y) {
    //printf("regenerate %d %d\n", x, y);
    ppu_off();
    growing_tree(x/2, (y-1)/2);
    render();
    screen_load_one(0);
    screen_load_one(1);
    ppu_on_all();
}
