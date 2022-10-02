#include "levels/levels.h"
#pragma rodata-name(push, "LVLDAT0")
const level_t level = {
    /* name= */ "             FOREST",
    /* objset= */ {
        // 0
        0x80,0x81,0x82,0x83,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,

        // 4
        0xa8,0xa9,0xaa,0xab,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,

        // 8
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,

        // 12
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,

        // 16
        0x84,0x85,0x86,0x87,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,

        // 20
        0xa0,0xa1,0xa2,0xa3,
        0xa4,0xa5,0xa6,0xa7,
        0x00,0x00,0x00,0x00,
        0xac,0xad,0xae,0xaf,
    },

    /* pallete= */ {
        // Background palette
        0x0f, 0x12, 0x16, 0x30,  // text: black, blue, red, white.
        0x0f, 0x17, 0x1a, 0x2a,  // forest: black, brown, green, light green.
        0x0f, 0x07, 0x17, 0x0f,  // black, brown, light brown, black
        0x0f,0x00,0x10,0x30,     // greys

        // Sprite palette
        0x0f,0x17,0x26,0x12,    // Player
        0x0f,0x0f,0x12,0x30,    // stuff 1
        0x0f,0x37,0x07,0x17,    // stuff 2
        0x0f,0x00,0x10,0x30,    // stuff 3
    },

    /* start= */ { 7, 24 },
    /* next= */ 1,
    /* floor= */ 0x40,
    /* fill= */ 0x50,
};
#pragma rodata-name(pop)