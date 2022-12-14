#ifndef LD51_LEVELS_LEVELS_H
#define LD51_LEVELS_LEVELS_H
#include <stdint.h>

typedef struct level {
    uint8_t name[32];
    uint8_t objset[256];
    uint8_t palette[32];
    struct {
        uint8_t x, y;
    } start;
    uint8_t next;
    uint8_t floor;
    uint8_t fill;
    uint8_t song;
} level_t;

extern const level_t level;

#endif // LD51_LEVELS_LEVELS_H
