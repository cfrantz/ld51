#include <stdint.h>
#include <stdio.h>

#include "neslib/util/random.h"

extern void write();
const void* const useless_decl_to_force_linking[] = {
    (const void const*)write,
};

void main() {
    rand_seed(0);
    for (;;) {
        uint16_t val = rand16();
        printf("val = %5u\n", val);
    }
}
