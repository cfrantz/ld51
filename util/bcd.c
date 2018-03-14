#include <stdint.h>

uint16_t __fastcall__ bcd_add16(uint16_t a, uint16_t b) {
    static uint16_t t1, t2;

    t1 = a + 0x0666;
    t2 = t1 ^ b;
    t1 = t1 + b;
    t2 = t1 ^ t2;
    t2 = ~t2 & 0x1110;
    t2 = (t2>>2) | (t2>>3);
    return t1 - t2;
}

uint16_t __fastcall__ bcd_sub16(uint16_t a, uint16_t b) {
    b = 0x9999 - b;
    return bcd_add16(a, bcd_add16(b, 1));
}
