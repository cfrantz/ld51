.include "neslib/globals.inc"
.import popa, popax

.export _ppu_macro_ascii_uint16

prdec16_tens:
    .word   1
    .word   10
    .word   100
    .word   1000
    .word   10000

; void __fastcall__ ppu_macro_ascii_uint16(uint16_t b);
; Always 5 digits, always zero padded.

_ppu_macro_ascii_uint16:
    sta     ntmp+0
    stx     ntmp+1
    
    ldy     #8
prdec16_loop1:
    ldx     #$ff
    sec
prdec16_loop2:
    lda     ntmp+0
    sbc     prdec16_tens+0,y
    sta     ntmp+0
    lda     ntmp+1
    sbc     prdec16_tens+1,y
    sta     ntmp+1
    inx
    bcs     prdec16_loop2

    lda     ntmp+0
    adc     prdec16_tens+0,y
    sta     ntmp+0
    lda     ntmp+1
    adc     prdec16_tens+1,y
    sta     ntmp+1
    txa
    ora     #$30

    ldx     _ppu_macro_len
    sta     _ppu_macro,x
    inx
    stx     _ppu_macro_len
   
    dey
    dey
    bpl     prdec16_loop1
    rts
