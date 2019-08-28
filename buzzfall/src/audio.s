init_sound:
; Init all APU registers to off and silent
    ldy #$13
@loop:  
	lda sound_init_data, y
    sta $4000, y
    dey
    bpl @loop
 
    ; We have to skip over $4014 (OAMDMA)
    lda #$0f
    sta $4015
    lda #$40
    sta $4017
    rts
   
sound_init_data:
    .byte $30,$08,$00,$00
    .byte $30,$08,$00,$00
    .byte $80,$00,$00,$00
    .byte $30,$00,$00,$00
    .byte $00,$00,$00,$00

