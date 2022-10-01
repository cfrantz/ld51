.include "neslib/globals.inc"
.include "neslib/nes.inc"
.import popax

.export _scroll0
.export _scroll1

;; The commented version below is the mmc3 implementation.
;; Each mapper package should implement this routine in the
;; appropriate way for that mapper.

;; ;void __fastcall__ set_split(unsigned char ypos);
;; _set_split:
;;     sta $c000
;;     sta $c001
;;     cmp #0
;;     beq @1
;;     sta $e001
;;     rts
;; @1:
;;     sta $e000
;;     rts

;void __fastcall__ scroll0(unsigned int x,unsigned int y);
_scroll0:
	sta ntmp

	txa
	bne @1
	lda ntmp
	cmp #240
	bcs @1
	sta scroll_y0
	lda #0
	sta ntmp
	beq @2	;bra

@1:
	sec
	lda ntmp
	sbc #240
	sta scroll_y0
	lda #2
	sta ntmp
@2:

	jsr popax
	sta scroll_x0
	txa
	and #$01
	ora ntmp
	sta ntmp
	lda ppu_ctrl_var
	and #$fc
	ora ntmp
	sta ppu_ctrl_var
	rts

;void __fastcall__ scroll1(unsigned int x,unsigned int y);
_scroll1:
	sta ntmp

	txa
	bne @1
	lda ntmp
	cmp #240
	bcs @1
	sta scroll_y1
	lda #0
	sta ntmp
	beq @2	;bra

@1:
	sec
	lda ntmp
	sbc #240
	sta scroll_y1
	lda #2
	sta ntmp
@2:

	jsr popax
    ; Pre-compute the values we'll write to the PPU_ADDR register to
    ; adjust the x/y scroll.
    ; https://wiki.nesdev.com/w/index.php/PPU_scrolling

    ; The nametable is stored as (NT << 2)
	sta scroll_x1
	txa
	and #$01
	ora ntmp
    asl
    asl
    sta scroll_nt

    ; The PPU_ADDR is computed as ((X & 0xF8) >> 3 | (Y & 0x38) << 2)
    lda scroll_x1
    lsr
    lsr
    lsr
    sta ntmp
    lda scroll_y1
    and #$38
    asl
    asl
    ora ntmp
    sta scroll_addr
	rts
