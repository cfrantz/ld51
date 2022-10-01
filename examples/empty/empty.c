#include <stdint.h>

#define writereg(addr, val) (*(volatile uint8_t*)addr = val)
void main() {
    const char *hello = "Hello World\n";
    while(*hello) {
        writereg(0x4018, *hello);
        hello++;
    }
    
    // No exit, spin forever.
    for(;;);
}
