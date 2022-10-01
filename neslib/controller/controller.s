.include "neslib/globals.inc"
.include "neslib/nes.inc"
.import popa, popax

.export _pad_poll
.export _pad_trigger
.export _pad_state

.ZEROPAGE
ctrl_pad_state:         .res 2
ctrl_pad_state_trig:    .res 2

.BSS
ctrl_pad_buf:           .res 3
ctrl_pad_state_prev:    .res 2

.CODE
;unsigned char __fastcall__ pad_poll(unsigned char pad);
_pad_poll:
	tay
	ldx #0

@padPollPort:
	lda #1
	sta CTRL_PORT1
	lda #0
	sta CTRL_PORT1
	lda #8
	sta ntmp

@padPollLoop:
	lda CTRL_PORT1,y
	lsr a
	ror ctrl_pad_buf,x
	dec ntmp
	bne @padPollLoop

	inx
	cpx #3
	bne @padPollPort

	lda ctrl_pad_buf
	cmp ctrl_pad_buf+1
	beq @done
	cmp ctrl_pad_buf+2
	beq @done
	lda ctrl_pad_buf+1
@done:
	sta ctrl_pad_state,y
	tax
	eor ctrl_pad_state_prev,y
	and ctrl_pad_state ,y
	sta ctrl_pad_state_trig,y
	txa
	sta ctrl_pad_state_prev,y
	rts


;unsigned char __fastcall__ pad_trigger(unsigned char pad);

_pad_trigger:
	pha
	jsr _pad_poll
	pla
	tax
	lda ctrl_pad_state_trig,x
	rts


;unsigned char __fastcall__ pad_state(unsigned char pad);

_pad_state:
	tax
	lda ctrl_pad_state,x
	rts
