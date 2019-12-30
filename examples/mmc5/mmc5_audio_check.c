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

const uint8_t peroid_lo[] = {
    0xf1,0x7f,0x13,0xad,0x4d,0xf3,0x9d,0x4c,0x00,0xb8,0x74,0x34,
    0xf8,0xbf,0x89,0x56,0x26,0xf9,0xce,0xa6,0x80,0x5c,0x3a,0x1a,
    0xfb,0xdf,0xc4,0xab,0x93,0x7c,0x67,0x52,0x3f,0x2d,0x1c,0x0c,
    0xfd,0xef,0xe1,0xd5,0xc9,0xbd,0xb3,0xa9,0x9f,0x96,0x8e,0x86,
    0x7e,0x77,0x70,0x6a,0x64,0x5e,0x59,0x54,0x4f,0x4b,0x46,0x42,
    0x3f,0x3b,0x38,0x34,0x31,0x2f,0x2c,0x29,0x27,0x25,0x23,0x21,
    0x1f,0x1d,0x1b,0x1a,0x18,0x17,0x15,0x14,
};

const uint8_t peroid_hi[] = {
    0x07,0x07,0x07,0x06,0x06,0x05,0x05,0x05,0x05,0x04,0x04,0x04,
    0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

const uint8_t init_values[] = {
    0x30,0x08,0x00,0x00,
    0x30,0x08,0x00,0x00,
    0x80,0x00,0x00,0x00,
    0x30,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
};

const char note_names[][4] = {
    "A1", "A#1", "B1", "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1",
    "A2", "A#2", "B2", "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2",
    "A3", "A#3", "B3", "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3",
    "A4", "A#4", "B4", "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4",
    "A5", "A#5", "B5", "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5",
    "A6", "A#6", "B6", "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6",
    "A7", "A#7", "B7", "C7", "C#7", "D7", "D#7", "E7",
};

void apu_init(void) {
    uint8_t y;

    for(y=0; y<sizeof(init_values); ++y) {
        writereg8(0x4000+y, init_values[y]);
    }
    // turn on all 4 nes audio channels.
    writereg8(0x4015, 0x0f);
    // turn on the mmc5 audio channels.
    writereg8(0x5015, 0x0f);
    writereg8(0x4017, 0x40);
}

const uint16_t apu_regs[] = {
    // triangle, apu pulse, apu pulse, mmc5 pulse, mmc5 pulse
    0x4008, 0x4000, 0x4004, 0x5000, 0x5004
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
            writereg8(apu_regs[chan]+2, peroid_lo[note]);
            writereg8(apu_regs[chan]+3, peroid_hi[note]);
            writereg8(apu_regs[chan]+0, 0xC0);
        }
    } else {
        if (note == 0xFF) {
            // Silence
            writereg8(apu_regs[chan]+0, 0x30);
        } else {
            duty = (duty << 6 | 0x3f);
            writereg8(apu_regs[chan]+2, peroid_lo[note]);
            writereg8(apu_regs[chan]+3, peroid_hi[note]);
            writereg8(apu_regs[chan]+0, duty);
        }
    }
}

uint16_t framenum;
uint8_t player_pad;
uint8_t player_pad_changed;
uint8_t pad;
uint8_t val;

uint8_t notes[5];
uint8_t noteon[5];
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

    vram_puts(10, 3, "Hello World!");
    vram_puts(9, 12, "APU Demo");
    vram_puts(0, 14, "TRI P1  P2  M1  M2");

    apu_init();
    notes[0] = 15 - 12;
    notes[1] = 15;
    notes[2] = 15 + 4;
    notes[3] = 15 + 7;
    notes[4] = 15 + 12;

    noteon[0] = 1;
    noteon[1] = 1;
    noteon[2] = 1;
    noteon[3] = 1;
    noteon[4] = 1;

    for(val=0; val<5; ++val) {
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
            if (note_num < 4) ++note_num;
        }
        ppu_macro_word(0x2000 + 16*32 + note_num*4);
        ppu_macro_byte(1);
        ppu_macro_byte('^');

        // If up/down was pressed, change the note being played.
        if (pad & (PAD_UP | PAD_DOWN | PAD_A)) {
            if (pad & PAD_UP) {
                ++notes[note_num];
            } else if (pad & PAD_DOWN) {
                --notes[note_num];
            } else if (pad & PAD_A) {
                noteon[note_num] = !noteon[note_num];
            }

            apu_play(note_num,
                     noteon[note_num] ?  notes[note_num] : 0xFF,
                     2);

            ppu_macro_word(0x2000 + 15*32 + note_num*4);
            ppu_macro_byte(3);
            if (noteon[note_num]) {
                ppu_macro_byte(note_names[notes[note_num]][0]);
                ppu_macro_byte(note_names[notes[note_num]][1]);
                ppu_macro_byte(note_names[notes[note_num]][2]);
            } else {
                ppu_macro_byte('-');
                ppu_macro_byte('-');
                ppu_macro_byte('-');
            }
        }

    }
}

