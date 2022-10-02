#ifndef LD43_GAME_ENTITY_H
#define LD43_GAME_ENTITY_H
#include <stdint.h>


extern uint8_t framenum;
extern uint8_t player_pad;
extern uint8_t player_pad_changed;
extern uint8_t spridx;

extern void entity_move(void);
extern void player_input(void);
extern void player_display(void);
extern uint8_t player_check_exit(void);

extern void player_init(uint8_t x, uint8_t y);
extern void player_init_once(uint8_t x, uint8_t y);
extern void entity_all(void);
extern void entity_load_screen(uint8_t dmap);
#endif // LD43_GAME_ENTITY_H
