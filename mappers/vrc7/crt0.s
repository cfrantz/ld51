; Startup code for cc65 and Shiru's NES library
; based on code by Groepaz/Hitmen <groepaz@gmx.net>, Ullrich von Bassewitz <uz@cc65.org>


NES_MAPPER				=85	;mapper number
NES_PRG_BANKS			=4	;number of 16K PRG banks, change to 2 for NROM256
NES_CHR_BANKS			=1	;number of 8K CHR banks
NES_MIRRORING			=1	;0 horizontal, 1 vertical, 8 four screen


.define SPEED_FIX		0	;zero if you want to handle PAL/NTSC speed difference by yourself


    .export _exit,__STARTUP__:absolute=1
    .export _vrc7_reg
    .export _set_split
    .export nmi, irq, vrc7boot
	.import initlib,push0,popa,popax,_main,zerobss,copydata

    .import _cfplayer_init
    .import _cfplayer_update_frame

	.import _pal_bright
	.import _pal_clear
	.import _oam_clear

; Linker generated symbols
	.import __RAM_START__   ,__RAM_SIZE__
	.import __ROM0_START__  ,__ROM0_SIZE__
	.import __STARTUP_LOAD__,__STARTUP_RUN__,__STARTUP_SIZE__
	.import	__CODE_LOAD__   ,__CODE_RUN__   ,__CODE_SIZE__
	.import	__RODATA_LOAD__ ,__RODATA_RUN__ ,__RODATA_SIZE__

    .include "zeropage.inc"
    .include "neslib/globals.inc"
    .include "neslib/nes.inc"

.segment "HEADER"

    .byte $4e,$45,$53,$1a
	.byte NES_PRG_BANKS
	.byte NES_CHR_BANKS
	.byte NES_MIRRORING|(NES_MAPPER<<4)&$f0
	.byte NES_MAPPER&$f0
	.res 8,0


.segment "STARTUP"

    .byte "STARTUP"
start:
_exit:

    sei
    ldx #$ff
    txs
    stx PPU_FRAMECNT
    inx
    stx PPU_MASK
    stx DMC_FREQ
    stx PPU_CTRL		;no NMI

;    lda #$41
;    sta $4018
;    lda #$0a
;    sta $4018

waitSync1:
    bit PPU_STATUS
@1:
    bit PPU_STATUS
;    bpl @1

clearPalette:
	ldy #$3f
	sty PPU_ADDR
	stx PPU_ADDR
	lda #$0f
	ldx #$20
@1:
	sta PPU_DATA
	dex
	bne @1

clearVRAM:
	txa
	ldy #$20
	sty PPU_ADDR
	sta PPU_ADDR
	ldy #$10
@1:
	sta PPU_DATA
	inx
	bne @1
	dey
	bne @1

clearRAM:
    txa
@1:
    sta $000,x
    sta $100,x
    sta $200,x
    sta $300,x
    sta $400,x
    sta $500,x
    sta $600,x
    sta $700,x
    inx
    bne @1

	lda #4
	jsr _pal_bright
	jsr _pal_clear
	jsr _oam_clear

    jsr	zerobss
	jsr	copydata

    lda #<(__RAM_START__+__RAM_SIZE__)
    sta	sp
    lda	#>(__RAM_START__+__RAM_SIZE__)
    sta	sp+1            ; Set argument stack ptr

	jsr	initlib

waitSync2:
    bit PPU_STATUS
@1:
    bit PPU_STATUS
    bpl @1

	lda #%10000000
	sta ppu_ctrl_var
	sta PPU_CTRL		;enable NMI
	lda #%00000110
	sta ppu_mask_var

waitSync3:
	lda _frame_count
@1:
	cmp _frame_count
	beq @1

	lda #0
	sta PPU_SCROLL
	sta PPU_SCROLL

    jsr _cfplayer_init
    ; enable interrupts
    cli
	jmp _main			;no parameters

.ZEROPAGE
vrc7_irq_counter: .res 1

.segment "VRC7BOOT"

;NMI handler
nmi:
	pha
	txa
	pha
	tya
	pha

    lda ppu_mask_var        ; Is the background on?
    and #$08
    beq @oam_update         ; If no, we dont' want an IRQ
    lda vrc7_irq_counter
    beq @oam_update
    sta $e010               ; Lines to wait
    lda #2
    sta $f000               ; Enable, scanline mode, no reload after ack
@oam_update:
	ldx #$00
	stx PPU_OAM_ADDR
	lda #>OAM_BUF
	sta PPU_OAM_DMA

	lda pal_update
    bne @palette_update
    jmp @exec_ppu_macro

@palette_update:
	lda #$3f
	sta PPU_ADDR
	stx PPU_ADDR
	stx pal_update

	.repeat 4,I
	ldy pal_buf+I
	lda (pal_ptr),y
	sta PPU_DATA
	.endrepeat

	.repeat 7,J
	lda PPU_DATA			;skip background color
	.repeat 3,I
	ldy pal_buf+5+(J*4)+I
	lda (pal_ptr),y
	sta PPU_DATA
	.endrepeat
	.endrepeat

@exec_ppu_macro:

	ldx _ppu_macro_len
	beq @updates_done
	ldy #0
@updName:
	lda _ppu_macro,y
    iny
    sta PPU_ADDR
	lda _ppu_macro,y
    iny
    sta PPU_ADDR

	lda _ppu_macro,y
    and #$80
    bne @updVert
	lda ppu_ctrl_var
    and #$fb
    bne @updDir
@updVert:
	lda ppu_ctrl_var
    ora #$04
@updDir:
    sta PPU_CTRL
	lda _ppu_macro,y
    iny
    and #$7f
    tax

@updPPU:
	lda _ppu_macro,y
    iny
	sta PPU_DATA
	dex
	bne @updPPU
    
    cpy _ppu_macro_len
    bne @updName
	ldy #0
    sty _ppu_macro_len
    ;.byte $f2

@updates_done:
	stx PPU_ADDR
	stx PPU_ADDR

    ; Set the region0 scroll registers
    lda scroll_x0
    sta PPU_SCROLL
    lda scroll_y0
    sta PPU_SCROLL
    lda ppu_ctrl_var
    sta PPU_CTRL

	inc _frame_count
    bne @framecount_done
	inc _frame_count+1
@framecount_done:

    ; cfplayer eats a lot of cpu time, so re-enable interrupts
    ; so the scanline interrupt can always happen on time.
    cli
    jsr _cfplayer_update_frame
@nmiexit:
	pla
	tay
	pla
	tax
	pla
    rti

irq:
    pha
	txa
	pha
	tya
	pha
    ; ack the irq
    sta $f010

    ldx #13
@hline_delay:
    dex
    bne @hline_delay

    lda $2002           ;reset the ppu toggle
    ; Order of writes from "Split X/Y scroll" section on
    ; https://wiki.nesdev.com/w/index.php/PPU_scrolling
	lda scroll_nt
	sta PPU_ADDR
	lda scroll_y1
	sta PPU_SCROLL
	lda scroll_x1
    ldy scroll_addr

;    ldx #7
;@hline_delay:
;    dex
;    bne @hline_delay

	sta PPU_SCROLL
    sty PPU_ADDR
@irqexit:
    pla
    tay
    pla
    tax
    pla
    rti

;void __fastcall__ set_split(unsigned char ypos);
_set_split:
    cmp #0
    beq @1
    sta ntmp
    lda #256-20
    sec
    sbc ntmp
@1:
    sta vrc7_irq_counter
    rts

;void __fastcall__ vrc7_reg(uint8_t reg, uint8_t val)

; Arbitrary choice of register order:
; Regs 0-7 control CHR
; Regs 8,9,10 control PRG banks
vrc7_reg_table_hi:
    .byte >$A000
    .byte >$A010
    .byte >$B000
    .byte >$B010

    .byte >$C000
    .byte >$C010
    .byte >$D000
    .byte >$D010

    .byte >$8000
    .byte >$8010
    .byte >$9000
vrc7_reg_table_lo:
    .byte <$A000
    .byte <$A010
    .byte <$B000
    .byte <$B010

    .byte <$C000
    .byte <$C010
    .byte <$D000
    .byte <$D010

    .byte <$8000
    .byte <$8010
    .byte <$9000
    
_vrc7_reg:
    pha
    jsr popa
    tax
    pla

vrc7_reg:
    pha
    lda vrc7_reg_table_lo,x
    sta ntmp
    lda vrc7_reg_table_hi,x
    sta ntmp+1
    ldy #0
    pla
    sta (ntmp),y
    rts
    
vrc7boot:
    sei
    ldx #$ff
    txs
    inx
    stx PPU_MASK
    stx DMC_FREQ
    stx PPU_CTRL		; no NMI

    ; Setup CHR banks in registers 0-7
;    ldx #7
;@chr_loop:
;    txa
;    jsr vrc7_reg
;    dex
;    bpl @chr_loop
    ldx #0
    stx $A000
    inx
    stx $A010
    inx
    stx $B000
    inx
    stx $B010
    inx
    stx $C000
    inx
    stx $C010
    inx
    stx $D000
    inx
    stx $D010

    ldx #8
    lda #0
    jsr vrc7_reg

    inx
    lda #253
    jsr vrc7_reg

    inx
    lda #254
    jsr vrc7_reg

    ; Enable WRAM at $6000; horizontal mirroring
    lda #$81
    sta $e000
    jmp start

.segment "VECTORS"

    .word nmi	;$fffa vblank nmi
    .word vrc7boot	;$fffc reset
   	.word irq	;$fffe irq / brk
