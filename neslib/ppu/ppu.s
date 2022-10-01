.include "neslib/globals.inc"
.include "neslib/nes.inc"
.import popa, popax

.export _ppu_off
.export _ppu_on_all
.export _ppu_on_bg
.export _ppu_on_spr
.export _ppu_mask
.export _ppu_waitnmi
.export _bank_spr
.export _bank_bg
.export _vram_read
.export _vram_write
.export _ppu_macro_byte
.export _ppu_macro_word
.export _vram_adr
.export _vram_put
.export _vram_fill
.export _vram_inc

;void __fastcall__ ppu_off(void);
_ppu_off:
	lda ppu_mask_var
	and #%00011000
	beq @1
	jsr _ppu_waitnmi
@1:
	lda ppu_mask_var
	and #%11100111
	sta ppu_mask_var
	sta PPU_MASK
	lda #0
	sta PPU_CTRL
	rts

;void __fastcall__ ppu_on_all(void);
_ppu_on_all:
	lda ppu_mask_var
	ora #%00011000
ppu_onoff:
	sta ppu_mask_var
	sta PPU_MASK
	lda #$80
	sta PPU_CTRL
	jsr _ppu_waitnmi
	lda #$00
	sta PPU_ADDR
	sta PPU_ADDR
	lda ppu_ctrl_var
	sta PPU_CTRL
	rts


;void __fastcall__ ppu_on_bg(void);
_ppu_on_bg:
	lda ppu_mask_var
	ora #%00001000
	bne ppu_onoff	;bra


;void __fastcall__ ppu_on_spr(void);
_ppu_on_spr:
	lda ppu_mask_var
	ora #%00010000
	bne ppu_onoff	;bra


;void __fastcall__ ppu_mask(unsigned char mask);
_ppu_mask:
	sta ppu_mask_var
	sta PPU_MASK
	rts


;void __fastcall__ ppu_waitnmi(void);
_ppu_waitnmi:
	lda _frame_count
@1:
	cmp _frame_count
	beq @1
	rts


;void __fastcall__ bank_spr(unsigned char n);
_bank_spr:
	and #$01
	asl a
	asl a
	asl a
	sta ntmp
	lda ppu_ctrl_var
	and #%11110111
	ora ntmp
	sta ppu_ctrl_var
	rts



;void __fastcall__ bank_bg(unsigned char n);
_bank_bg:
	and #$01
	asl a
	asl a
	asl a
	asl a
	sta ntmp
	lda ppu_ctrl_var
	and #%11101111
	ora ntmp
	sta ppu_ctrl_var
	rts



;void __fastcall__ vram_read(unsigned char *dst,unsigned int adr,unsigned int size);
_vram_read:
	sta ntmp
	stx ntmp+1

	jsr popax
	stx PPU_ADDR
	sta PPU_ADDR
	lda PPU_DATA

	jsr popax
	sta ntmp+2
	stx ntmp+3

	ldy #0
@1:
	lda PPU_DATA
	sta (ntmp+2),y
	inc ntmp+2
	bne @2
	inc ntmp+3
@2:
	lda ntmp
	bne @3
	dec ntmp+1
@3:
	dec ntmp
	lda ntmp
	ora ntmp+1
	bne @1

	rts



;void __fastcall__ vram_write(unsigned char *src,unsigned int adr,unsigned int size);
_vram_write:
	sta ntmp
	stx ntmp+1

	jsr popax
	stx PPU_ADDR
	sta PPU_ADDR

	jsr popax
	sta ntmp+2
	stx ntmp+3

	ldy #0
@1:
	lda (ntmp+2),y
	sta PPU_DATA
	inc ntmp+2
	bne @2
	inc ntmp+3
@2:
	lda ntmp
	bne @3
	dec ntmp+1
@3:
	dec ntmp
	lda ntmp
	ora ntmp+1
	bne @1

	rts


; void __fastcall__ ppu_macro_byte(uint8_t b);
_ppu_macro_byte:
    ldy     _ppu_macro_len
    sta     _ppu_macro,y
    iny
    sty     _ppu_macro_len
    rts

; void __fastcall__ ppu_macro_word(uint16_t b);
_ppu_macro_word:
    ldy     _ppu_macro_len
    sta     _ppu_macro+1,y
    txa
    sta     _ppu_macro,y
    iny
    iny
    sty     _ppu_macro_len
    rts

;void __fastcall__ vram_adr(unsigned int adr);
_vram_adr:
	stx PPU_ADDR
	sta PPU_ADDR
	rts



;void __fastcall__ vram_put(unsigned char n);
_vram_put:
	sta PPU_DATA
	rts



;void __fastcall__ vram_fill(unsigned char n,unsigned int len);
_vram_fill:
	sta nlen
	stx nlen+1
	jsr popa
	ldx nlen+1
	beq @2
	ldx #0
@1:
	sta PPU_DATA
	dex
	bne @1
	dec nlen+1
	bne @1
@2:
	ldx nlen
	beq @4
@3:
	sta PPU_DATA
	dex
	bne @3
@4:
	rts



;void __fastcall__ vram_inc(unsigned char n);
_vram_inc:
	beq @1
	lda #$04
@1:
	sta ntmp
	lda ppu_ctrl_var
	and #$fb
	ora ntmp
	sta ppu_ctrl_var
	sta PPU_CTRL
	rts
