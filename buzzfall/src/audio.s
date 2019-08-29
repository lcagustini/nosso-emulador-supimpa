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

InitMusic:
    lda #$00
    ldx #$00
Clear_Sound:
    sta $4000,x
    inx
    cpx #$0F
    bne Clear_Sound

    lda #$10
    sta $4010
    lda #$00
    sta $4011
    sta $4012
    STA $4013

    lda #%00001111
    STA $4015

    lda #$C0
    STA $4017

    ;LDA SongNumber      ; song number
    ldx #$00            ; 00 for NTSC or $01 for PAL
    jsr INITADDY        ; init address
    rts