#include <stdint.h>
#include "neslib/util/debug.h"
#include "util/compiler.h"

// Supply a 'write' function so cc65's printf goes to my debug port
int fastcall write (int fd, const char* buf, unsigned count) {
    uint8_t i;
    (void)fd;
    for(i=0; i<count; ++i) {
        xputc(buf[i]);
    }
    return i;
}

void fastcall xputs(const char *str) {
    uint8_t i;
    for(i=0; str[i]; ++i) {
        xputc(str[i]);
    }
}
