.include "neslib/globals.inc"

.export _rand_seed
.export _rand8
.export _rand16

.BSS
seed:                   .res 2

;; A simple Galois LFSR with period 65534.
;; If the user attempts to seed with 0, it is seeded with the
;; current 16-bit (_frame_count | 1).

.CODE
;void __fastcall__ rand_seed(uint16_t seed);
_rand_seed:
    cmp #0
    bne @save
    cpx #0
    bne @save
    lda _frame_count
    ora #1
    ldx _frame_count+1    
@save:
    sta seed
    stx seed+1
    rts

;uint16_t __fastcall__ rand16();
;uint8_t __fastcall__ rand8(uint16_t seed);
_rand16:
    ldx seed+1
_rand8:
    lsr seed+1
    ror seed
    lda #0
    bcc @clkdone
    lda #$b4
    eor seed+1
    sta seed+1
@clkdone:
    lda seed
    rts
