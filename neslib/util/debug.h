#ifndef EMPTYNES_NESLIB_UTIL_DEBUG_H
#define EMPTYNES_NESLIB_UTIL_DEBUG_H
#include "neslib/util/registers.h"

#define xputc(val_) register_write8(0x4018, val_)
void fastcall xputs(const char *str);

#endif  // EMPTYNES_NESLIB_UTIL_DEBUG_H
