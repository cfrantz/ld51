#pragma data-name(push, "CHARS")

#if 0
#define XCHR(a, b, c, d, e, f, g, h) \
        a, b, c, d, e, f, g, h, \
        a, b, c, d, e, f, g, h
#else
// Main char is color 3, drop shadow color 1.
#define XCHR(a, b, c, d, e, f, g, h) \
        a, b | (a>>1), c | (b>>1), d | (c>>1), e | (d>>1), f | (e>>1), g | (f>>1),  h | (g>>1), \
        a, b, c, d, e, f, g, h
#endif

unsigned char mcr[256 * 8 * 2] = {
#include "ascii-ctrl.h"
#include "mcr-symdigit.h"
#include "mcr-upper.h"
#include "mcr-lower.h"
#include "unscii-pseudogfx.h"
};
