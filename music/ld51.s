.export _song_table
_song_table:
    .WORD _ld51
    .WORD 0
    .WORD 0
    .WORD 0

.export _drum_period
_drum_period:
    .BYTE $0e
    .BYTE $06
.export _drum_patch
_drum_patch:
    .BYTE $08
    .BYTE $10

.export _instruments_table
_instruments_table:
    ; Instrument name: 'piano', type: VRC7
    .WORD 0,0,0,$0003
    ; Instrument name: 'hat', type: 2A03
    .WORD env_hat_volume,0,0,0
    ; Instrument name: 'snare', type: 2A03
    .WORD env_snare_volume,0,0,0
    ; Instrument name: 'flute', type: VRC7
    .WORD 0,0,0,$0004
    ; Instrument name: 'bass', type: VRC7
    .WORD 0,0,0,$000d

env_hat_volume:
    .BYT $0c,$0b,$00,$a0,$60,$40,$30,$20,$20,$10,$10,$00
env_snare_volume:
    .BYT $15,$14,$00,$f0,$c0,$90,$80,$70,$60,$50,$40,$40,$30,$30,$30,$20,$20,$10,$10,$10,$00

.export _ld51
_ld51:
    .BYT 11, 0
    .WORD 0
    .WORD 0
    .WORD 0
    .WORD ld51_drumset_data
    .WORD 0
    .WORD ld51_piano_1_data
    .WORD ld51_piano_2_data
    .WORD ld51_piano_3_data
    .WORD ld51_piano_4_data
    .WORD ld51_violin_data
    .WORD ld51_bass_guitar_data
ld51_drumset_data:
    .WORD ld51_drumset_sequence
    .WORD ld51_drumset_measure1
    .WORD ld51_drumset_measure2
    .WORD ld51_drumset_measure3
ld51_drumset_sequence:
    .BYT  $01,$02,$02,$03,$02,$02,$02,$03,$02,$02,$02,$03,$02,$02,$02,$03,$80,$00
ld51_drumset_measure1:
    .BYT  $20,$00,$0a,$21,$e7,$22,$f3,$21,$f3,$21,$e6,$22,$e7,$11,$ff,$00
ld51_drumset_measure2:
    .BYT  $21,$e7,$22,$f3,$21,$f3,$21,$e6,$22,$e7,$11,$ff,$00
ld51_drumset_measure3:
    .BYT  $21,$e7,$22,$f3,$21,$f3,$21,$e6,$22,$f3,$22,$f4,$11,$ff,$00
ld51_piano_1_data:
    .WORD ld51_piano_1_sequence
    .WORD ld51_piano_1_measure1
    .WORD ld51_piano_1_measure2
    .WORD ld51_piano_1_measure3
    .WORD ld51_piano_1_measure4
    .WORD ld51_piano_1_measure5
ld51_piano_1_sequence:
    .BYT  $01,$02,$03,$04,$05,$02,$03,$04,$05,$02,$03,$04,$05,$02,$03,$04,$80,$00
ld51_piano_1_measure1:
    .BYT  $20,$00,$0a,$4c,$9f,$11,$fa,$00
ld51_piano_1_measure2:
    .BYT  $48,$9f,$11,$fa,$00
ld51_piano_1_measure3:
    .BYT  $4f,$9f,$11,$fa,$00
ld51_piano_1_measure4:
    .BYT  $4d,$9f,$11,$fa,$00
ld51_piano_1_measure5:
    .BYT  $4c,$9f,$11,$fa,$00
ld51_piano_2_data:
    .WORD ld51_piano_2_sequence
    .WORD ld51_piano_2_measure1
    .WORD ld51_piano_2_measure2
    .WORD ld51_piano_2_measure3
    .WORD ld51_piano_2_measure4
ld51_piano_2_sequence:
    .BYT  $01,$02,$03,$03,$04,$02,$03,$03,$04,$02,$03,$03,$04,$02,$03,$03,$80,$00
ld51_piano_2_measure1:
    .BYT  $20,$00,$0a,$48,$9f,$11,$fa,$00
ld51_piano_2_measure2:
    .BYT  $43,$9f,$11,$fa,$00
ld51_piano_2_measure3:
    .BYT  $4a,$9f,$11,$fa,$00
ld51_piano_2_measure4:
    .BYT  $48,$9f,$11,$fa,$00
ld51_piano_3_data:
    .WORD ld51_piano_3_sequence
    .WORD ld51_piano_3_measure1
    .WORD ld51_piano_3_measure2
    .WORD ld51_piano_3_measure3
    .WORD ld51_piano_3_measure4
    .WORD ld51_piano_3_measure5
ld51_piano_3_sequence:
    .BYT  $01,$02,$03,$04,$05,$02,$03,$04,$05,$02,$03,$04,$05,$02,$03,$04,$80,$00
ld51_piano_3_measure1:
    .BYT  $20,$00,$0a,$45,$9f,$11,$fa,$00
ld51_piano_3_measure2:
    .BYT  $40,$9f,$11,$fa,$00
ld51_piano_3_measure3:
    .BYT  $48,$9f,$11,$fa,$00
ld51_piano_3_measure4:
    .BYT  $47,$9f,$11,$fa,$00
ld51_piano_3_measure5:
    .BYT  $45,$9f,$11,$fa,$00
ld51_piano_4_data:
    .WORD ld51_piano_4_sequence
    .WORD ld51_piano_4_measure1
    .WORD ld51_piano_4_measure2
    .WORD ld51_piano_4_measure3
    .WORD ld51_piano_4_measure4
ld51_piano_4_sequence:
    .BYT  $01,$02,$03,$03,$04,$02,$03,$03,$04,$02,$03,$03,$04,$02,$03,$03,$80,$00
ld51_piano_4_measure1:
    .BYT  $20,$00,$0a,$41,$9f,$11,$fa,$00
ld51_piano_4_measure2:
    .BYT  $3c,$9f,$11,$fa,$00
ld51_piano_4_measure3:
    .BYT  $43,$9f,$11,$fa,$00
ld51_piano_4_measure4:
    .BYT  $41,$9f,$11,$fa,$00
ld51_violin_data:
    .WORD ld51_violin_sequence
    .WORD ld51_violin_measure1
    .WORD ld51_violin_measure2
    .WORD ld51_violin_measure3
    .WORD ld51_violin_measure4
    .WORD ld51_violin_measure5
    .WORD ld51_violin_measure6
    .WORD ld51_violin_measure7
    .WORD ld51_violin_measure8
    .WORD ld51_violin_measure9
    .WORD ld51_violin_measure10
    .WORD ld51_violin_measure11
    .WORD ld51_violin_measure12
ld51_violin_sequence:
    .BYT  $01,$02,$03,$04,$05,$06,$07,$08,$09,$0a,$0b,$0c,$09,$0a,$0b,$0c,$80,$00
ld51_violin_measure1:
    .BYT  $20,$18,$20,$18,$20,$18,$0a,$45,$da,$48,$f3,$4c,$f3,$4d,$f3,$4c,$f3,$4d,$f4,$11,$ff,$00
ld51_violin_measure2:
    .BYT  $4c,$f4,$4a,$f3,$4c,$f3,$4a,$f3,$48,$f3,$43,$f3,$40,$f3,$41,$f4,$11,$ff,$00
ld51_violin_measure3:
    .BYT  $43,$da,$45,$f3,$48,$e6,$4a,$f3,$48,$f4,$11,$ff,$00
ld51_violin_measure4:
    .BYT  $47,$e7,$4f,$f3,$4d,$f3,$4c,$f3,$4a,$f3,$48,$f3,$47,$f4,$11,$ff,$00
ld51_violin_measure5:
    .BYT  $45,$da,$48,$f3,$4c,$f3,$4d,$f3,$4c,$f3,$4d,$f4,$11,$ff,$00
ld51_violin_measure6:
    .BYT  $4c,$f4,$4a,$f3,$4c,$f3,$4a,$f3,$48,$e6,$45,$fa,$48,$f9,$4c,$fa,$4d,$fa,$11,$ff,$00
ld51_violin_measure7:
    .BYT  $4f,$cd,$4f,$f3,$56,$f3,$54,$fa,$53,$f9,$51,$fa,$4d,$fa,$11,$ff,$00
ld51_violin_measure8:
    .BYT  $4f,$cd,$4c,$f3,$4a,$f3,$47,$f3,$45,$f4,$11,$ff,$00
ld51_violin_measure9:
    .BYT  $41,$b3,$40,$f3,$3e,$f4,$11,$ff,$00
ld51_violin_measure10:
    .BYT  $3c,$cd,$3c,$f3,$3e,$f3,$40,$f3,$41,$f4,$11,$ff,$00
ld51_violin_measure11:
    .BYT  $43,$cd,$43,$f3,$4a,$f3,$43,$f3,$42,$ff,$43,$ff,$44,$ff,$45,$ff,$46,$ff,$47,$ff,$48,$ff,$49,$ff,$4a,$ff,$11,$ff,$11,$ff,$4e,$ff,$11,$ff,$00
ld51_violin_measure12:
    .BYT  $4f,$9a,$11,$ff,$00
ld51_bass_guitar_data:
    .WORD ld51_bass_guitar_sequence
    .WORD ld51_bass_guitar_measure1
    .WORD ld51_bass_guitar_measure2
    .WORD ld51_bass_guitar_measure3
    .WORD ld51_bass_guitar_measure4
    .WORD ld51_bass_guitar_measure5
    .WORD ld51_bass_guitar_measure6
    .WORD ld51_bass_guitar_measure7
    .WORD ld51_bass_guitar_measure8
    .WORD ld51_bass_guitar_measure9
    .WORD ld51_bass_guitar_measure10
    .WORD ld51_bass_guitar_measure11
    .WORD ld51_bass_guitar_measure12
ld51_bass_guitar_sequence:
    .BYT  $01,$02,$03,$04,$05,$02,$03,$04,$06,$07,$08,$09,$06,$0a,$0b,$0c,$80,$00
ld51_bass_guitar_measure1:
    .BYT  $20,$20,$20,$20,$20,$20,$0a,$35,$b3,$34,$f3,$32,$f4,$11,$ff,$00
ld51_bass_guitar_measure2:
    .BYT  $30,$cd,$30,$f3,$32,$f3,$34,$f3,$35,$f4,$11,$ff,$00
ld51_bass_guitar_measure3:
    .BYT  $37,$cd,$37,$f3,$3e,$f3,$37,$f3,$36,$ff,$37,$ff,$38,$ff,$39,$ff,$3a,$ff,$3b,$ff,$3c,$ff,$3d,$ff,$3e,$ff,$11,$ff,$11,$ff,$42,$ff,$11,$ff,$00
ld51_bass_guitar_measure4:
    .BYT  $43,$9a,$11,$ff,$00
ld51_bass_guitar_measure5:
    .BYT  $35,$b3,$34,$f3,$32,$f4,$11,$ff,$00
ld51_bass_guitar_measure6:
    .BYT  $2d,$da,$30,$f3,$34,$f3,$35,$f3,$34,$f3,$35,$f4,$11,$ff,$00
ld51_bass_guitar_measure7:
    .BYT  $34,$f4,$32,$f3,$34,$f3,$32,$f3,$30,$f3,$2b,$f3,$28,$f3,$29,$f4,$11,$ff,$00
ld51_bass_guitar_measure8:
    .BYT  $2b,$da,$2d,$f3,$30,$e6,$32,$f3,$30,$f4,$11,$ff,$00
ld51_bass_guitar_measure9:
    .BYT  $2f,$e7,$37,$f3,$35,$f3,$34,$f3,$32,$f3,$30,$f3,$2f,$f4,$11,$ff,$00
ld51_bass_guitar_measure10:
    .BYT  $34,$f4,$32,$f3,$34,$f3,$32,$f3,$30,$e6,$2d,$fa,$30,$f9,$34,$fa,$35,$fa,$11,$ff,$00
ld51_bass_guitar_measure11:
    .BYT  $37,$cd,$37,$f3,$3e,$f3,$3c,$fa,$3b,$f9,$39,$fa,$35,$fa,$11,$ff,$00
ld51_bass_guitar_measure12:
    .BYT  $37,$cd,$34,$f3,$32,$f3,$2f,$f3,$2d,$f4,$11,$ff,$00

