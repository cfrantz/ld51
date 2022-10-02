#ifndef LD51_MUSIC_CFPLAYER_H
#define LD51_MUSIC_CFPLAYER_H
#include <stdint.h>
#include "util/compiler.h"

void fastcall cfplayer_init(void);
void fastcall cfplayer_silence(void);
void fastcall cfplayer_set_pause(uint8_t pause);
void fastcall cfplayer_reset_song(void);
void fastcall cfplayer_update_frame(void);

extern uint16_t cfplayer_now_playing;
#pragma zpsym("cfplayer_now_playing");
extern uint16_t sfx_now_playing;

#endif // LD51_MUSIC_CFPLAYER_H
