#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "util/nesutil.h"

const unsigned char palette[32]={
    // Background palette
    0x22, 0x03, 0x23, 0x30, // skyblue, dark purple, med purple, white
    0x0f, 0x11, 0x21, 0x30, // black, blue, blue, white
    0x0f, 0x07, 0x17, 0x0f, // black, brown, light brown, black
    0x0f, 0x0a, 0x1a, 0x0f, // black, green, light green, black

    // Sprite palette
    0x0F, 0x02, 0x07, 0x26, // xx, blue, brown, pink
    0x0F, 0x16, 0x2d, 0x0f, // xx, light red, gray, black
    0x0F, 0x02, 0x11, 0x20, // xx, blue, light blue, white
    0x0F, 0x2c, 0x28, 0x30, // xx, cyan, yellow, white
};


#define CPU 178977300UL
#define F_MULT 16UL
#define T(f) (uint16_t)((CPU / (F_MULT * f)) - 1)
#define FROW(octave, A, Bb, B, C, Db, D, Eb, E, F, Gb, G, Ab) \
    T(octave * A), \
    T(octave * Bb), \
    T(octave * B), \
    T(octave * C), \
    T(octave * Db), \
    T(octave * D), \
    T(octave * Eb), \
    T(octave * E), \
    T(octave * F), \
    T(octave * Gb), \
    T(octave * G), \
    T(octave * Ab)

#define FREQROW(octave) \
    FROW((1UL<<octave), \
            5500, \
            5827, 6174, \
            6541, \
            6930, 7342, \
            7778, 8241, \
            8731, \
            9250, 9800, \
            10383)

const uint16_t period[] = {
    FREQROW(0), FREQROW(1), FREQROW(2), FREQROW(3),
    FREQROW(4), FREQROW(5), FREQROW(6),
};

#undef F_MULT
#define F_MULT 14UL

const uint16_t vrc6_saw_period[] = {
    FREQROW(0), FREQROW(1), FREQROW(2), FREQROW(3),
    FREQROW(4), FREQROW(5), FREQROW(6),
};

const uint8_t init_values[] = {
    0x30,0x08,0x00,0x00,
    0x30,0x08,0x00,0x00,
    0x80,0x00,0x00,0x00,
    0x30,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
};

const char note_names[][4] = {
    "A1", "A#1", "B1", "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2",
    "A2", "A#2", "B2", "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3",
    "A3", "A#3", "B3", "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4",
    "A4", "A#4", "B4", "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5",
    "A5", "A#5", "B5", "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6",
    "A6", "A#6", "B6", "C7", "C#7", "D7", "D#7", "E7", "F7", "F#7", "G7", "G#7",
    "A7", "A#7", "B7", "C8", "C#8", "D8", "D#8", "E8", "F8", "F#8", "G8", "G#8",
};

void apu_init(void) {
    uint8_t y;

    for(y=0; y<sizeof(init_values); ++y) {
        writereg8(0x4000+y, init_values[y]);
    }
    // turn on all 4 nes audio channels.
    writereg8(0x4015, 0x0f);
    writereg8(0x4017, 0x40);
}

const uint16_t apu_regs[] = {
    // triangle, apu pulse, apu pulse, mmc5 pulse, mmc5 pulse
    0x4008, 0x4000, 0x4004, 0x9000, 0xA000, 0xB000
};

void apu_play(uint8_t chan, uint8_t note, uint8_t duty) {
    if (chan == 0) {
        if (note == 0xFF) {
            // Silence
            writereg8(apu_regs[chan], 0x80);
        } else {
            // The triangle wave is clocked at half the speed, so adjust
            // for it naturally being an octave lower.
            note += 12;
            writereg8(apu_regs[chan]+2, period[note] & 0xFF);
            writereg8(apu_regs[chan]+3, period[note] >> 8);
            writereg8(apu_regs[chan]+0, 0xC0);
        }
    } else if (chan < 3) {
        if (note == 0xFF) {
            // Silence
            writereg8(apu_regs[chan]+0, 0x30);
        } else {
            duty = (duty << 6 | 0x3f);
            writereg8(apu_regs[chan]+2, period[note] & 0xFF);
            writereg8(apu_regs[chan]+3, period[note] >> 8);
            writereg8(apu_regs[chan]+0, duty);
        }
    } else if (chan < 5) {
        if (note == 0xFF) {
            // Silence
            writereg8(apu_regs[chan]+0, 0);
        } else {
            duty = (duty << 4 | 0x0F);
            writereg8(apu_regs[chan]+1, period[note] & 0xFF);
            writereg8(apu_regs[chan]+2, 0x80 | period[note] >> 8);
            writereg8(apu_regs[chan]+0, duty);
        }
    } else {
        if (note == 0xFF) {
            // Silence
            writereg8(apu_regs[chan]+0, 0);
        } else {
            duty = (duty << 4 | 0x0F);
            writereg8(apu_regs[chan]+1, vrc6_saw_period[note] & 0xFF);
            writereg8(apu_regs[chan]+2, 0x80 | vrc6_saw_period[note] >> 8);
            writereg8(apu_regs[chan]+0, duty);
        }
    }
}

uint16_t framenum;
uint8_t player_pad;
uint8_t player_pad_changed;
uint8_t pad;
uint8_t val;

uint8_t notes[6];
uint8_t duty[6];
uint8_t note_num;

// supply an empty FamiToneUpdate, overiding the routine in famitone.s.
// this allows us to program the audio devices without the real famitone
// player stomping on us.
void FamiToneUpdate() {}

void main(void)
{
    bank_bg(0);
    bank_spr(1);
    ppu_off();
    pal_all(palette); //set palette for sprites
    oam_size(1);

    vram_puts(9, 3, "APU+VRC6 Demo");
    vram_puts(0, 5, "L/R: change channel");
    vram_puts(0, 6, "U/D: change note");
    vram_puts(0, 7, "SELECT: change duty cycle");
    vram_puts(0, 8, "T=Triangle, P=Pulse");
    vram_puts(0, 13, "TRI AP1 AP2 VP1 VP2 VT1");
    vram_puts(0, 14, "D0  D0  D0  D0  D0  D0");

    apu_init();
    notes[0] = 15 - 12;
    notes[1] = 15;
    notes[2] = 15 + 4;
    notes[3] = 15 + 7;
    notes[4] = 15 + 12;
    notes[5] = 15 + 12 + 12;

    for(val=0; val<sizeof(notes); ++val) {
        apu_play(val, notes[val], 2);
        ppu_macro_word(0x2000 + 15*32 + val*4);
        ppu_macro_byte(3);
        ppu_macro_byte(note_names[notes[val]][0]);
        ppu_macro_byte(note_names[notes[val]][1]);
        ppu_macro_byte(note_names[notes[val]][2]);
    }

    ppu_on_all();
    for(framenum=0;;++framenum) {
        ppu_waitnmi();
        oam_clear();
        player_pad_changed = pad_trigger(0);
        player_pad = pad_state(0);

        pad = player_pad & player_pad_changed;

        // overwrite the cursor
        ppu_macro_word(0x2000 + 16*32 + note_num*4);
        ppu_macro_byte(1);
        ppu_macro_byte(' ');
        if (pad & PAD_LEFT) {
            if (note_num > 0) --note_num;
        } else if (pad & PAD_RIGHT) {
            if (note_num < sizeof(notes)-1) ++note_num;
        }
        ppu_macro_word(0x2000 + 16*32 + note_num*4);
        ppu_macro_byte(1);
        ppu_macro_byte('^');

        // If up/down was pressed, change the note being played.
        if (pad & (PAD_UP | PAD_DOWN | PAD_SELECT)) {
            if (pad & PAD_UP) {
                ++notes[note_num];
            } else if (pad & PAD_DOWN) {
                --notes[note_num];
            }
            if (pad & PAD_SELECT) {
                switch(note_num) {
                    case 1:
                    case 2:
                        duty[note_num] = (duty[note_num] + 1) & 3;
                        break;
                    case 3:
                    case 4:
                        duty[note_num] = (duty[note_num] + 1) & 7;
                        break;
                    default:
                        ;
                }
            }

            apu_play(note_num, notes[note_num], duty[note_num]);
            ppu_macro_word(0x2000 + 15*32 + note_num*4);
            ppu_macro_byte(3);
            ppu_macro_byte(note_names[notes[note_num]][0]);
            ppu_macro_byte(note_names[notes[note_num]][1]);
            ppu_macro_byte(note_names[notes[note_num]][2]);
            ppu_macro_word(0x2001 + 14*32 + note_num*4);
            ppu_macro_byte(1);
            ppu_macro_byte(0x30+duty[note_num]);
        }

    }
}

