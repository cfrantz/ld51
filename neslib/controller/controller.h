#ifndef EMPTYNES_NESLIB_CONTROLLER_PAD_H_
#define EMPTYNES_NESLIB_CONTROLLER_PAD_H_
#include <stdint.h>
#include "util/compiler.h"

#define PAD_A			0x01
#define PAD_B			0x02
#define PAD_SELECT		0x04
#define PAD_START		0x08
#define PAD_UP			0x10
#define PAD_DOWN		0x20
#define PAD_LEFT		0x40
#define PAD_RIGHT		0x80

uint8_t fastcall pad_poll(uint8_t pad);
uint8_t fastcall pad_trigger(uint8_t pad);
uint8_t fastcall pad_state(uint8_t pad);

#endif  // EMPTYNES_NESLIB_CONTROLLER_PAD_H_
