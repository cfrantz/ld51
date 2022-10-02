;==============================================================================
; Exported symbols
;==============================================================================

.export _cfplayer_init
.export _cfplayer_silence
.export _cfplayer_set_pause
.export _cfplayer_reset_song
.export _cfplayer_update_frame
.export _cfplayer_now_playing
.export _sfx_now_playing

;==============================================================================
; Exports for debug
;==============================================================================
.IFDEF CFPLAYER_DEBUG
.export process_envelopes
.export process_envelope_state
.export load_envelope_ptr
.export channel_delay
.export channel_seq_pos
.export channel_meas_pos
.export channel_note
.export channel_volume
.export channel_instrument
.export channel_env_state
.export channel_env_vol
.export channel_env_arp
.export channel_env_pitch
.export channel_env_duty
.export channel_owner
.ENDIF

;==============================================================================
; Imported symbols
;==============================================================================
.import _instruments_table
.import _drum_period
.import _drum_patch

; The drum note numbers get remapped starting at note $21.  Subtract $21
; so we load from the start of each table.
drum_period = _drum_period - $21
drum_patch = _drum_patch - $21

; Number of channels:
; NES:
;  0 = Pulse 0
;  1 = Pulse 1
;  2 = Triangle
;  3 = Noise
;  4 = DMC
;  5 = MMC5 Pulse 0
;  6 = MMC5 Pulse 1

VRC7_CHANNEL = 5
VRC7_SEL = $9010
VRC7_DATA = $9030
;  5 = VRC7:0
;  6 = VRC7:1
;  7 = VRC7:2
;  8 = VRC7:3
;  9 = VRC7:4
;  10 = VRC7:5

NUM_CHANNELS=11
NUM_SFX=4

;==============================================================================
; Opcodes in the measure "command list"
;==============================================================================
; $00: terminator (end of measure)
; $01 - $10: set volume
; $11: note off
; $12 - $1F: undefined
; $20 <byte>: set instrument to <byte>
; $21 - $77: note on
; $80 - $FF: delay (two's compliment, count up to zero).
NOTE_OFF = $11
SONG_END = $12
NEXT_SONG = $13
RESTART_LAST_SONG = $14
TITLE_SCREEN_HACK = $15
PROGRAM_CHANGE = $20

;==============================================================================
; Opcodes in the SFX "command list"
;==============================================================================
; $00: terminator (end of measure)
; $01: ZP write
; $02: Cancel currently playing song.
; $4x: Write to APU register $4000+x
; $80 - $FF: delay (two's compliment, count up to zero).

;==============================================================================
; Channel envelope states
;==============================================================================
ENV_OFF = 0
ENV_ON = 2
ENV_RELEASE = 3
ENV_RELEASING = 4
;==============================================================================
; Envelope pointer indicies
;==============================================================================
ENV_VOLUME = 0
ENV_ARPEGGIO = 2
ENV_PITCH = 4
ENV_DUTY = 6

.ZEROPAGE
_cfplayer_now_playing:  .res 2
ptr1:                   .res 2
ptr2:                   .res 2
tmp1:                   .res 1
tmp2:                   .res 1

.BSS
_bss_start:
channel_delay:          .res NUM_CHANNELS
channel_seq_pos:        .res NUM_CHANNELS
channel_meas_pos:       .res NUM_CHANNELS
channel_note:           .res NUM_CHANNELS
channel_volume:         .res NUM_CHANNELS
channel_instrument:     .res NUM_CHANNELS
channel_env_state:      .res NUM_CHANNELS   ; state: ENV_ constants.
channel_env_vol:        .res NUM_CHANNELS   ; position within vol envelope
channel_env_arp:        .res NUM_CHANNELS   ; position within arp envelope
channel_env_pitch:      .res NUM_CHANNELS   ; position within pitch envelope
channel_env_duty:       .res NUM_CHANNELS   ; position within duty envelope
channel_owner:          .res NUM_CHANNELS   ; whether music or SFX own the chan

_sfx_delay:             .res NUM_SFX
_sfx_now_playing:       .res NUM_SFX*2

apu_shadow_ctrl:        .res 1
apu_shadow_tlo:         .res 1
apu_shadow_thi:         .res 1
paused:                 .res 1
_bss_length = * - _bss_start
apu_thi_prev:           .res NUM_CHANNELS

.CODE

;==============================================================================
; Delay between VRC7 register writes
;
; According to https://wiki.nesdev.org/w/index.php?title=VRC7_audio,
; writes to the VRC7 data register must be followed by at least a
; 42-clock delay.
;
; Writes to the VRC7 select register must be followed by at least a
; 6-clock delay.
;==============================================================================
vrc7_data_delay:
                    ; 6 clocks: jsr to get here
    pha             ; 3 clocks
    lda     $8000   ; 4 clocks
    lda     $8000   ; 4 clocks
    lda     $8000   ; 4 clocks
    lda     $8000   ; 4 clocks
    lda     $8000   ; 4 clocks
    lda     $8000   ; 4 clocks
    pla             ; 4 clocks
vrc7_sel_delay:
    rts             ; 6 clocks
                    ; 43 clocks total

.macro write_vrc7_sel register
    .if (.match({register}, a))
        sta VRC7_SEL
    .elseif (.match({register}, x))
        stx VRC7_SEL
    .elseif (.match({register}, y))
        sty VRC7_SEL
    .else
        .error "Syntax error"
    .endif
        jsr vrc7_sel_delay
.endmacro

.macro write_vrc7_data register
    .if (.match({register}, a))
        sta VRC7_DATA
    .elseif (.match({register}, x))
        stx VRC7_DATA
    .elseif (.match({register}, y))
        sty VRC7_DATA
    .else
        .error "Syntax error"
    .endif
        jsr vrc7_data_delay
.endmacro

;==============================================================================
; Initialize the player
;
;==============================================================================
.proc _cfplayer_init
    lda     #0
    sta     _cfplayer_now_playing
    sta     _cfplayer_now_playing+1
    ldx     #_bss_length
init_mem_loop:
    dex
    sta     _bss_start,x
    bne     init_mem_loop

    lda     #$FF
    ldx     #NUM_CHANNELS
init_thi_prev:
    dex
    sta     apu_thi_prev,x
    bne     init_thi_prev

    jsr     _cfplayer_silence
    lda     #$1f                ; All APU channels enabled
    sta     $4015
    lda     #3                  ; MMC5 Pulse channels enabled
    sta     $5015
    rts
.endproc

;==============================================================================
; Silence all sound channels
;
;==============================================================================
.proc _cfplayer_silence
    lda     #$30                    ; Pulse/Noise zero volume value
    sta     $4000
    sta     $4004
    sta     $400c
    sta     $5000
    sta     $5004
    lda     #$80                    ; triangle zero volume value
    sta     $4008
    jsr     _vrc7_silence
    rts
.endproc

.proc _vrc7_silence
    ldx     #VRC7_CHANNEL
vrc7_loop:
    lda     vrc_reg_3,x
    write_vrc7_sel a
    lda     channel_instrument,x
    ora     #$F
    write_vrc7_data a
    inx
    cpx     #NUM_CHANNELS
    bne     vrc7_loop
    rts
.endproc

.proc _vrc7_unsilence
    ldx     #VRC7_CHANNEL
vrc7_loop:
    lda     vrc_reg_3,x
    write_vrc7_sel a
    ldy     channel_volume,x
    lda     vrc7_volume_table,y
    ora     channel_instrument,x
    write_vrc7_data a
    inx
    cpx     #NUM_CHANNELS
    bne     vrc7_loop
    rts
.endproc

;==============================================================================
; Pause or un-pause the player
; Entry:
;   Desired pause state in A (1=paused, 0=playing)
; Return:
;   Z flag:
;       0 - nothing to do
;       1 - run player
;==============================================================================
.proc _cfplayer_set_pause
    tax                         ; copy paused state
    dex                         ; invert in X: 0=playing, FF=pause
    cmp     paused              ; equal to current paused state?
    beq     done                ; yes, nothing to do
    sta     paused              ; no, update paused state
    txa                         ; X->A for Z-flag side effect
    beq     pause
unpause:
    jsr     _vrc7_unsilence
    ldx     #$1f                ; All APU channels enabled
    stx     $4015
    ldx     #3                  ; MMC5 Pulse channels enabled
    stx     $5015
    bne     done
pause:
    jsr     _vrc7_silence
    ldx     #0                  ; Silence at the enable bits
    stx     $4015
    stx     $5015
    stx     apu_thi_prev+3      ; zap the noise length counter so we re-write it when we unpause
done:
    txa                         ; X->A for Z-flag side effect
    rts
.endproc

;==============================================================================
; Reset player variables for processing a song
;
;==============================================================================
.proc _cfplayer_reset_song
    jsr     _cfplayer_silence
    lda     #0
    ldx     #NUM_CHANNELS
loop:
    dex
    sta     channel_delay,x
    sta     channel_seq_pos,x
    sta     channel_meas_pos,x
    sta     channel_instrument,x
    sta     channel_env_state,x
    sta     channel_note,x
    sta     channel_volume,x
    bne     loop
    rts
.endproc

;==============================================================================
; Update the sound engine for this frame
;
; Play pending sfx first, then music.
;==============================================================================
.proc _cfplayer_update_frame
    jsr play_sfx_frame
    jsr play_music_frame
    rts
.endproc

;==============================================================================
; Play music for this frame.
;
;==============================================================================
.proc play_music_frame
    ; must remain valid during the entire music player.
    nr_tracks = tmp2

    lda     _cfplayer_now_playing+1       ;high-byte of now_playing pointer
    beq     done                    ; if it's zero, it can't be a song
    ldy     #0
    ldx     #0
    lda     (_cfplayer_now_playing),y     ; number of tracks
    sta     nr_tracks
loop:
    txa                             ; X is the track number.
    asl                             ; Convert track number to ptr offset
    tay                             ; (track = (track * 2) + 2
    iny                             ;
    iny
    lda     (_cfplayer_now_playing),y     ; track ptr low
    sta     ptr1+0
    iny
    lda     (_cfplayer_now_playing),y     ; track ptr high
    sta     ptr1+1
    beq     next                    ; it it's zero, it can't be a valid track
    cpx     #4                      ; DMC channel?
    beq     next                    ; No DMC impl for now.
    jsr     play_music_channel      ; Play music for this track
next:
    inx
    cpx     nr_tracks
    bne     loop
done:
    rts
.endproc

;==============================================================================
; Play music for a particular channel.
; Channel number in X
; Destroys A, Y.
;==============================================================================
.proc play_music_channel
    inc     channel_delay,x         ; new event?
    bpl     load_sequence_pos       ; positive: process event
                                    ; negative: delay
done:
    jsr     process_envelopes
    rts
load_sequence_pos:
    ldy     #0                      ; Read seq ptr from track ptr.
    lda     (ptr1),y                ; seq ptr low
    sta     ptr2
    iny
    lda     (ptr1),y                ; seq ptr hi
    sta     ptr2+1
    lda     channel_seq_pos,x       ; where are we in the sequence?
load_sequence:
    tay
    lda     (ptr2),y                ; measure
    bpl     load_measure            ; positive values: measure number.
    and     #$7f                    ; negative value: loop to (value&0x7f).
    sta     channel_seq_pos,x       ; go to that sequence position
    bpl     load_sequence           ; branch always: will be positive after AND.
load_measure:
    beq     done                    ; zero terminator? yes: done.
    asl                             ; measure number to pointer offset
    tay
    lda     (ptr1),y                ; measure ptr low
    sta     ptr2
    iny
    lda     (ptr1),y                ; measure ptr hi
    sta     ptr2+1

load_music_event:
    ldy     channel_meas_pos,x
    inc     channel_meas_pos,x
    lda     (ptr2),y                ; get note or event.
    bpl     event_or_terminator
    sta     channel_delay,x         ; negative value: delay.
    bne     done
event_or_terminator:
    bne     process_event           ; zero terminator?
    inc     channel_seq_pos,x       ; yes: next sequence position
    lda     #0
    sta     channel_meas_pos,x      ; reset measure position to 0
    beq     load_sequence_pos
process_event:
    cmp     #$21                    ; is this a note event?
    bcs     note_event
    cmp     #PROGRAM_CHANGE         ; is this a program change event?
    bne     next1
    jsr     program_change
    inc     channel_meas_pos,x      ; increment measure pos (should never wrap)
    bne     load_music_event        ; get next music event
next1:
.IFDEF ZELDA2_INTEGRATION
    cmp     #SONG_END
    bne     next2
    lda     #0                      ; Tell the game the song has ended
    sta     $07FB
    beq     load_music_event
next2:
    cmp     #NEXT_SONG
    bne     next3
    iny                             ; next song is next byte
    lda     (ptr2),y                ; get song number
    sta     $EB                     ; Zelda2 command byte
    inc     channel_meas_pos,x      ; increment measure pos (should never wrap)
    bne     load_music_event        ; get next music event
next3:
    cmp     #RESTART_LAST_SONG
    bne     next4
    lda     $07DB
    sta     $EB                     ; Zelda2 command byte
    bne     load_music_event        ; get next music event
next4:
    cmp     #TITLE_SCREEN_HACK
    bne     next5
    iny                             ; value is next byte
    lda     (ptr2),y                ; get vlue
    sta     $E8                     ; Zelda2 title screen hack byte
    inc     channel_meas_pos,x      ; increment measure pos (should never wrap)
    bne     load_music_event        ; get next music event
next5:
.ENDIF
    cmp     #NOTE_OFF               ; NoteOff and ...
    bcs     note_off_event          ; ... all undefined events are NoteOff
    and     #$0f                    ; events $01-$10 are volume events
    sta     channel_volume,x
    bcc     load_music_event        ; carry should still be clear from prior cmp
note_off_event:
    lda     #ENV_RELEASE            ; Set the state to "release"
    sta     channel_env_state,x
    bne     load_music_event        ; get next music event
note_event:
    sta     channel_note,x          ; Save the note
    lda     #ENV_ON                 ; Set the state to NoteOn
    sta     channel_env_state,x
    sta     channel_env_vol,x       ; Reset envelopes:
    sta     channel_env_arp,x       ; ENV_ON happens to be the correct value.
    sta     channel_env_pitch,x
    sta     channel_env_duty,x
    jmp     load_music_event        ; get next music event
.endproc


.macro push16 addr
    lda     addr+0
    pha
    lda     addr+1
    pha
.endmacro

.macro pop16 addr
    pla
    sta     addr+1
    pla
    sta     addr+0
.endmacro

.proc program_change
    iny                             ; program to change to is next byte
    lda     (ptr2),y                ; get program number
    sta     channel_instrument,x
    cpx     #VRC7_CHANNEL
    bcc     done
    push16  ptr2
    lda     #6                      ; VRC7 instruments reuse the duty ptr to hold the patch number
    jsr     load_envelope_ptr
    beq     patch_num               ; invalid ptr is just a patch number
    ldy     #7                      ; valid ptr is a custom VRC7 patch; 8 bytes
custom_patch:
    lda     (ptr2),y                ; load the custom patch
    write_vrc7_sel y
    write_vrc7_data a
    dey
    bpl     custom_patch
    lda     #0                      ; custom patch is patch number 0
    beq     save_patch_num
patch_num:
    lda     ptr2+0
save_patch_num:
    asl                             ; shift into position for VRC7 register $3x
    asl
    asl
    asl
    sta     channel_instrument,x
    pop16   ptr2
done:
    rts
.endproc

;==============================================================================
; Process an instrument envelope
; Envelope number * 2 in A (0,2,4,6)
; Channel in X
;==============================================================================
.proc load_envelope_ptr
    ora     channel_instrument,x
    tay
    lda     _instruments_table,y     ; envelope ptr low
    sta     ptr2+0
    iny
    lda     _instruments_table,y     ; envelope ptr high
    beq     done                    ; it it's zero, it can't be a valid envelope
    sta     ptr2+1
done:
    rts
.endproc

;==============================================================================
; Advance envelope state
; Envelope pos in A
; Channel in X
; Returns new env pos in A, or zero if end of envelope.
;
; Note: envelope state: 0-off, 1-on, 2-rel, 3-releasing
; Envelope data layout: <len>, <loopidx>, <releaseidx>, <data...>
;==============================================================================
.proc process_envelope_state
    cmp     #2                      ; First valid env index is 3.
    bcs     check_state             ; We permit 2 because we start at index "-1"
    lda     #0
    rts
check_state:
    ldy     channel_env_state,x     ; state: 0-off, 1-on, 2-rel, 3-releasing
    beq     done                    ; state off, envelope done
check_on:
    cpy     #ENV_ON                 ; ENV_ON happens to be 2 which is also the index of the release point
    bne     check_release
    cmp     (ptr2),y                ; at release point?
    beq     loop_point              ; yes, go to loop point
    ldy     #0
    clc
    adc     #1                      ; increment envelope position
    cmp     (ptr2),y                ; at or beyond end point?
    bcs     loop_point              ; yes, go to loop point
    rts
loop_point:
    ldy     #1
    lda     (ptr2),y                ; load the loop point
    rts
check_release:
    cpy     #ENV_RELEASE            ; ENV_RELEASE happens to be 3, which is one past the index of the release point
    bne     do_releasing
    dey
    lda     (ptr2),y                ; load release point
    rts
do_releasing:
    ldy     #0
    cmp     (ptr2),y                ; at or beyond envelope end?
    bcs     done                    ; yes, envelope done
    clc
    adc     #1                      ; increment envelope position
    rts
done:
    tya
    rts
.endproc

;==============================================================================
; Process envelopes and store music data to apu shadow registers
; Channel in X
;==============================================================================
.proc process_envelopes
    cpx     #VRC7_CHANNEL
    bcc     check_drums
    jmp     process_vrc7
check_drums:    
    cpx     #3                      ; Drum channel?
    bne     volume
    ldy     channel_note,x
    lda     drum_patch,y
    sta     channel_instrument,x
volume:
    lda     #ENV_VOLUME
    jsr     load_envelope_ptr       ; load the pointer
    bne     volume_envelope         ; not null, process
no_volume_env:
    ldy     channel_env_state,x     ; Is the state is "on"?
    cpy     #ENV_ON
    bne     volume_value            ; No, env volume value is zero
    lda     #$F0                    ; Yes, env volume value is max
    bne     volume_value
volume_envelope:
    lda     channel_env_vol,x
    jsr     process_envelope_state
    sta     channel_env_vol,x
    tay
    beq     volume_value            ; Invalid index -> volume 0.
    lda     (ptr2),y                ; get volume value from envelope
volume_value:
    ora     channel_volume,x        ; multiply env vol by chan vol
    tay
    lda     volume_table,y          ; multiply using lookup table
    cpx     #2                      ; triangle channel
    bne     volume_store
    tay                             ; save volume value
    lda     #$80                    ; triangle zero volume value
    cpy     #0
    beq     volume_store
    lda     #$FF                    ; triangle "on" volume value
volume_store:
    sta     apu_shadow_ctrl
volume_done:

duty:
    cpx     #2                      ; no duty for triangle
    beq     duty_done
    lda     #ENV_DUTY
    jsr     load_envelope_ptr       ; load the pointer
    beq     duty_value              ; if null no processing
duty_envelope:
    lda     channel_env_duty,x
    jsr     process_envelope_state
    sta     channel_env_duty,x
    beq     duty_value              ; Invalid index -> duty setting 0
duty_index:
    tay
    lda     (ptr2),y                ; get duty envelope value
duty_value:
    tay
    lda     duty_table,y            ; transform to apu regs pattern
    ora     apu_shadow_ctrl
    sta     apu_shadow_ctrl
duty_done:

arpeggio:
    lda     #ENV_ARPEGGIO
    jsr     load_envelope_ptr       ; load the pointer
    beq     arpeggio_value          ; if null, no processing
arpeggio_envelope:
    lda     channel_env_arp,x
    jsr     process_envelope_state
    sta     channel_env_arp,x
    beq     arpeggio_value          ; invalid arp index?
arpeggio_index:
    tay
    lda     (ptr2),y                ; get arp envelope value
arpeggio_value:
    clc
    adc     channel_note,x          ; arp value + note_value
    tay
    cpx     #3                      ; drum channel?
    bne     arpeggio_standard
    lda     #8                      ; Set the length counter in the noise channel
    sta     apu_shadow_thi
    lda     drum_period,y           ; Get the noise period
    sta     apu_shadow_tlo
    jmp     arpeggio_done
arpeggio_standard:
    lda     note_table_lsb,y
    sta     apu_shadow_tlo
    lda     note_table_msb,y
    sta     apu_shadow_thi
arpeggio_done:

pitch:
    lda     #ENV_PITCH
    jsr     load_envelope_ptr       ; load the pointer
    beq     pitch_done              ; if null, no processing
pitch_envelope:
    lda     channel_env_pitch,x
    jsr     process_envelope_state
    sta     channel_env_pitch,x
    beq     pitch_done              ; invalid pitch index?
pitch_index:
    tay
    ldy     #0
    lda     (ptr2),y                ; get pitch envelope value
    bpl     pitch_value             ; sign-extend pitch value
    ldy     #$FF
pitch_value:
    clc
    adc     apu_shadow_tlo
    sta     apu_shadow_tlo
    tya
    adc     apu_shadow_thi
    sta     apu_shadow_thi
pitch_done:

    lda     channel_env_state,x     ; Is the state is release?
    cmp     #ENV_RELEASE
    bne     done
    inc     channel_env_state,x     ; yes, release->releasing
done:

    ldy     channel_owner,x         ; does music own the channel?
    bne     regs_done               ; no: no writes to APU
    txa
    asl
    asl
    tay                             ; y = apu_chan * 4
    cpx     #5                      ; MMC5 channels?
    bcs     mmc5_regs               ; yes: branch
    lda     apu_shadow_ctrl
    sta     $4000,y                 ; volume, duty & control
    lda     #0
    sta     $4001,y                 ; sweep
    lda     apu_shadow_tlo
    sta     $4002,y                 ; timer low
    lda     apu_shadow_thi
    cmp     apu_thi_prev,x          ; same timer_hi value as last time?
    beq     regs_done               ; yes: skip to avoid phase reset
    sta     $4003,y                 ; timer high
    sta     apu_thi_prev,x
    rts
mmc5_regs:
    ; Note: MMC5 regs minus 20 because the channel number (5 or greater)
    ; is already offset by 20 bytes (chan 5 * 4 bytes).
    lda     apu_shadow_ctrl
    sta     $5000-20,y              ; volume, duty & control
    lda     #0
    sta     $5001-20,y              ; sweep
    lda     apu_shadow_tlo
    sta     $5002-20,y              ; timer low
    lda     apu_shadow_thi
    cmp     apu_thi_prev,x
    beq     regs_done
    sta     $5003-20,y              ; timer high
    sta     apu_thi_prev,x
regs_done:
    rts
.endproc

.proc process_vrc7
    ldy     channel_note,x          ; note_number

    lda     vrc_reg_2,x             ; register $2x is the freq msb & trigger bit
    write_vrc7_sel a
    lda     channel_env_state,x     ; we reuse env_state as the trigger value
    cmp     #ENV_ON                 ; just got a note_on event?
    bne     note_off
    dec     channel_env_state,x     ; "ENV_ON-1" has no meaning, so we'll skip this next time
    lda     vrc_reg_3,x             ; register $3x is the volume
    write_vrc7_sel a
    lda     #$f                     ; zero volume
    ora     channel_instrument,x
    write_vrc7_data a
    lda     vrc_reg_2,x             ; register $2x is the freq msb & trigger bit
    write_vrc7_sel a
    lda     #0
    ora     vrc7_notes_msb,y
    write_vrc7_data a
    jmp     note_on
note_off:
    cmp     #ENV_RELEASE            ; note_off event?
    bne     done
    lda     #0                      ; Clear the trigger bit
    beq     note_val
note_on:
    lda     #$10                    ; otherwise set the trigger bit
note_val: 
    ora     vrc7_notes_msb,y
    write_vrc7_data a

    lda     vrc_reg_1,x             ; register $1x is the freq lsb
    write_vrc7_sel a
    lda     vrc7_notes_lsb,y
    write_vrc7_data a

    lda     vrc_reg_3,x             ; register $3x is the volume
    write_vrc7_sel a
    ldy     channel_volume,x
    lda     vrc7_volume_table,y
    ora     channel_instrument,x
    write_vrc7_data a
done:
    rts
.endproc

;==============================================================================
; Play sound effects
;
; Higher numbered SFX channels have higher priority, so we start at the
; last channel and count down.
; SFX channels are arranged so that:
;   0 -> music (not an SFX channel)
;   1..4 -> SFX channels
; The "sfx_*" variables are arranged so that abs,x/y addressing just works.
;==============================================================================

sfx_delay = _sfx_delay-1
sfx_now_playing = _sfx_now_playing-2

.proc play_sfx_frame
    ldx #NUM_SFX                    ; last SFX channel
loop:
    txa
    asl
    tay                             ; y = x * 2
    lda     sfx_now_playing,y       ; copy the pointer to ptr1 in ZP
    sta     ptr1+0
    lda     sfx_now_playing+1,y
    beq     next                    ; if the hibyte is zero then no effect
    sta     ptr1+1

    jsr     process_sfx             ; process the SFX data

    txa
    asl
    tay                             ; y = x * 2
    lda     ptr1+0
    sta     sfx_now_playing,y       ; copy the ZP pointer back to RAM
    lda     ptr1+1
    sta     sfx_now_playing+1,y
next:
    dex                             ; next sfx channel
    bne     loop
done:
    rts
.endproc


.macro inc16 addr
    .local done
    inc     addr
    bne     done
    inc     addr+1
done:
.endmacro

;==============================================================================
; Process the SFX data and write to the apu shadow regs
;==============================================================================
.proc process_sfx
    reg_index = tmp1
    sfx_chan = tmp2

    inc     sfx_delay,x             ; time to process the next event?
    bmi     done                    ; negative means delay
loop:
    ldy     #0
    stx     sfx_chan                ; save SFX channel
    lda     (ptr1),y                ; next sfx byte
    bmi     save_delay              ; negative values are delays
    beq     terminate               ; zero byte is the terminator
    inc16   ptr1                    ; increment pointer
    cmp     #$02                    ; Stop current song
    beq     stop_current_song
    cmp     #$01                    ; Zero page write opcode
    beq     sfx_zp_write
    and     #$1F                    ; keep apu reg index
    sta     reg_index
    lsr
    lsr
    tay                             ; apu channel in Y
    lda     sfx_priority,x          ; priority in A
    cmp     channel_owner,y
    bcc     regs_done               ; if cur_sfx_chan < owner, done

    sta     channel_owner,x         ; take ownership of the channel
    ldy     #0
    lda     (ptr1),y                ; register value to write
    ldy     reg_index               ; apu register to write
    sta     $4000,y                 ; write to APU regs
    tya
    and     #3                      ; allow phase reset for SFX
    cmp     #3
    bne     regs_done
    lda     #$FF
    sta     apu_thi_prev,x
regs_done:
    ldx     sfx_chan                ; get SFX channel back into X
    inc     ptr1+0                  ; inc ptr and process next SFX byte
    bne     loop
    inc     ptr1+1
    bne     loop
sfx_zp_write:
    lda     (ptr1),y                ; Zero page address
    tax                             ; command op into X
    inc16   ptr1
    lda     (ptr1),y                ; value into A
    sta     $0,x
    jmp     regs_done
terminate:
.IFDEF ZELDA2_INTEGRATION
    sta     $07FB,x                 ; Mark effect done ($7fb because SFX channels are one-based)
.ENDIF
    sta     ptr1+1                  ; null out the pointer
    ldy     #NUM_CHANNELS-1
termloop:
    lda     sfx_priority,x
    cmp     channel_owner,y
    bne     termnext
    lda     #0
    sta     channel_owner,y         ; release ownership on channels
termnext:
    dey
    bpl     termloop
    rts
save_delay:
    sta     sfx_delay,x
    inc16   ptr1
done:
    rts

stop_current_song:
    jsr     _cfplayer_reset_song    ; clobbers A and X
    sta     _cfplayer_now_playing
    sta     _cfplayer_now_playing+1
    ldx     sfx_chan                ; get SFX channel back into X
    jmp     loop
.endproc

sfx_priority:
    ; Zelda 2 sound effects channel priorities.
    ;     XX, EC, ED, EE, EF
    .byte  0,  4,  1,  2,  3

; Note tables
note_table_lsb = _note_table_lsb - $21
_note_table_lsb:
        .byte                                              $f1, $7f, $13 ; Octave 0
        .byte $ad, $4d, $f3, $9d, $4c, $00, $b8, $74, $34, $f8, $bf, $89 ; Octave 1
        .byte $56, $26, $f9, $ce, $a6, $80, $5c, $3a, $1a, $fb, $df, $c4 ; Octave 2
        .byte $ab, $93, $7c, $67, $52, $3f, $2d, $1c, $0c, $fd, $ef, $e1 ; Octave 3
        .byte $d5, $c9, $bd, $b3, $a9, $9f, $96, $8e, $86, $7e, $77, $70 ; Octave 4
        .byte $6a, $64, $5e, $59, $54, $4f, $4b, $46, $42, $3f, $3b, $38 ; Octave 5
        .byte $34, $31, $2f, $2c, $29, $27, $25, $23, $21, $1f, $1d, $1b ; Octave 6
        .byte $1a, $18, $17, $15, $14, $13, $12, $11, $10, $0f, $0e, $0d ; Octave 7

note_table_msb = _note_table_msb - $21
_note_table_msb:
        .byte                                              $07, $07, $07 ; Octave 0
        .byte $06, $06, $05, $05, $05, $05, $04, $04, $04, $03, $03, $03 ; Octave 1
        .byte $03, $03, $02, $02, $02, $02, $02, $02, $02, $01, $01, $01 ; Octave 2
        .byte $01, $01, $01, $01, $01, $01, $01, $01, $01, $00, $00, $00 ; Octave 3
        .byte $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00 ; Octave 4
        .byte $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00 ; Octave 5
        .byte $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00 ; Octave 6
        .byte $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00 ; Octave 7

; Duty lookup table.
duty_table:
    .byte $30
    .byte $70
    .byte $b0
    .byte $f0


vrc7_notes_lsb = _vrc7_notes_lsb - $21
_vrc7_notes_lsb:
    .byte                                              $22, $33, $45 ; Octave 0
    .byte $ac, $b6, $c1, $cd, $d9, $e6, $f3, $02, $11, $22, $33, $45 ; Octave 1
    .byte $ac, $b6, $c1, $cd, $d9, $e6, $f3, $02, $11, $22, $33, $45 ; Octave 2
    .byte $ac, $b6, $c1, $cd, $d9, $e6, $f3, $02, $11, $22, $33, $45 ; Octave 3
    .byte $ac, $b6, $c1, $cd, $d9, $e6, $f3, $02, $11, $22, $33, $45 ; Octave 4
    .byte $ac, $b6, $c1, $cd, $d9, $e6, $f3, $02, $11, $22, $33, $45 ; Octave 5
    .byte $ac, $b6, $c1, $cd, $d9, $e6, $f3, $02, $11, $22, $33, $45 ; Octave 6
    .byte $ac, $b6, $c1, $cd, $d9, $e6, $f3, $02, $11, $22, $33, $45 ; Octave 7

vrc7_notes_msb = _vrc7_notes_msb - $21
_vrc7_notes_msb:
    .byte                                              $01, $01, $01 ; Octave 0
    .byte $02, $02, $02, $02, $02, $02, $02, $03, $03, $03, $03, $03 ; Octave 1
    .byte $04, $04, $04, $04, $04, $04, $04, $05, $05, $05, $05, $05 ; Octave 2
    .byte $06, $06, $06, $06, $06, $06, $06, $07, $07, $07, $07, $07 ; Octave 3
    .byte $08, $08, $08, $08, $08, $08, $08, $09, $09, $09, $09, $09 ; Octave 4
    .byte $0a, $0a, $0a, $0a, $0a, $0a, $0a, $0b, $0b, $0b, $0b, $0b ; Octave 5
    .byte $0c, $0c, $0c, $0c, $0c, $0c, $0c, $0d, $0d, $0d, $0d, $0d ; Octave 6
    .byte $0e, $0e, $0e, $0e, $0e, $0e, $0e, $0f, $0f, $0f, $0f, $0f ; Octave 7

vrc7_volume_table:
    .byte $f,$e,$d,$c,$b,$a,$9,$8,$7,$6,$5,$4,$3,$2,$1,$0

vrc_reg_1 = _vrc_reg_1 - VRC7_CHANNEL
_vrc_reg_1:
    .byte $10,$11,$12,$13,$14,$15
vrc_reg_2 = _vrc_reg_2 - VRC7_CHANNEL
_vrc_reg_2:
    .byte $20,$21,$22,$23,$24,$25
vrc_reg_3 = _vrc_reg_3 - VRC7_CHANNEL
_vrc_reg_3:
    .byte $30,$31,$32,$33,$34,$35

; Precomputed volume multiplication table (rounded but never to zero unless one of the value is zero).
; Load the 2 volumes in the lo/hi nibble and fetch.

volume_table:
    .byte $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
    .byte $00, $01, $01, $01, $01, $01, $01, $01, $01, $01, $01, $01, $01, $01, $01, $01
    .byte $00, $01, $01, $01, $01, $01, $01, $01, $01, $01, $01, $01, $02, $02, $02, $02
    .byte $00, $01, $01, $01, $01, $01, $01, $01, $02, $02, $02, $02, $02, $03, $03, $03
    .byte $00, $01, $01, $01, $01, $01, $02, $02, $02, $02, $03, $03, $03, $03, $04, $04
    .byte $00, $01, $01, $01, $01, $02, $02, $02, $03, $03, $03, $04, $04, $04, $05, $05
    .byte $00, $01, $01, $01, $02, $02, $02, $03, $03, $04, $04, $04, $05, $05, $06, $06
    .byte $00, $01, $01, $01, $02, $02, $03, $03, $04, $04, $05, $05, $06, $06, $07, $07
    .byte $00, $01, $01, $02, $02, $03, $03, $04, $04, $05, $05, $06, $06, $07, $07, $08
    .byte $00, $01, $01, $02, $02, $03, $04, $04, $05, $05, $06, $07, $07, $08, $08, $09
    .byte $00, $01, $01, $02, $03, $03, $04, $05, $05, $06, $07, $07, $08, $09, $09, $0a
    .byte $00, $01, $01, $02, $03, $04, $04, $05, $06, $07, $07, $08, $09, $0a, $0a, $0b
    .byte $00, $01, $02, $02, $03, $04, $05, $06, $06, $07, $08, $09, $0a, $0a, $0b, $0c
    .byte $00, $01, $02, $03, $03, $04, $05, $06, $07, $08, $09, $0a, $0a, $0b, $0c, $0d
    .byte $00, $01, $02, $03, $04, $05, $06, $07, $07, $08, $09, $0a, $0b, $0c, $0d, $0e
    .byte $00, $01, $02, $03, $04, $05, $06, $07, $08, $09, $0a, $0b, $0c, $0d, $0e, $0f
