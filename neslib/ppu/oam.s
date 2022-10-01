.include "neslib/globals.inc"
.include "neslib/nes.inc"
.include "zeropage.inc"

;void __fastcall__ oam_clear(void);
_oam_clear:
	ldx #0
	lda #$ff
@1:
	sta OAM_BUF,x
	inx
	inx
	inx
	inx
	bne @1
	rts


;void __fastcall__ oam_size(unsigned char size);
_oam_size:
	asl a
	asl a
	asl a
	asl a
	asl a
	and #$20
	sta ntmp
	lda ppu_ctrl_var
	and #$df
	ora ntmp
	sta ppu_ctrl_var
	rts
	
	
;unsigned char __fastcall__ oam_spr(unsigned char x,unsigned char y,unsigned char chrnum,unsigned char attr,unsigned char sprid);
_oam_spr:
	tax

	ldy #0		;four popa calls replacement
	lda (sp),y
	iny
	sta OAM_BUF+2,x
	lda (sp),y
	iny
	sta OAM_BUF+1,x
	lda (sp),y
	iny
	sta OAM_BUF+0,x
	lda (sp),y
	sta OAM_BUF+3,x

	lda <sp
	clc
	adc #4
	sta <sp
	bcc @1
	inc <sp+1
@1:

	txa
	clc
	adc #4
	rts


;unsigned char __fastcall__ oam_meta_spr(unsigned char x,unsigned char y,unsigned char sprid,const unsigned char *data);
_oam_meta_spr:
	sta nptr
	stx nptr+1

	ldy #2		;three popa calls replacement, performed in reversed order
	lda (sp),y
	dey
	sta ntmp+0
	lda (sp),y
	dey
	sta ntmp+1
	lda (sp),y
	tax

@1:
	lda (nptr),y		;x offset
	cmp #$80
	beq @2
	iny
	clc
	adc ntmp+0
	sta OAM_BUF+3,x
	lda (nptr),y		;y offset
	iny
	clc
	adc ntmp+1
	sta OAM_BUF+0,x
	lda (nptr),y		;tile
	iny
	sta OAM_BUF+1,x
	lda (nptr),y		;attribute
	iny
	sta OAM_BUF+2,x
	inx
	inx
	inx
	inx
	jmp @1
@2:

	lda <sp
	adc #2	;carry is always set here, so it adds 3
	sta <sp
	bcc @3
	inc <sp+1
@3:

	txa
	rts


;void __fastcall__ oam_hide_rest(unsigned char sprid);
_oam_hide_rest:
	tax
	lda #240
@1:
	sta OAM_BUF,x
	inx
	inx
	inx
	inx
	bne @1
	rts
