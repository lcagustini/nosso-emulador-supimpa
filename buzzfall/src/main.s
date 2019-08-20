.segment "HEADER"
.byte "NES"                       ; signature
.byte $1A                         ; signature
.byte $02                         ; # of 16kb PRG-ROM banks
.byte $01                         ; # of 8kb VROM banks
.byte $00                         ; ROM control byte one
.byte $00                         ; ROM control byte two
.byte $00                         ; # of 8kb RAM banks
.byte $00                         ; reserved

.segment "VECTORS"
.addr nmi
.addr reset
.addr irq

; how we use ZEROPAGE section ??????
.segment "ZEROPAGE"               ; is this really supposed to be used like this? what about rs = 1?
bgPointerLo:  .byte 0             ; pointer variables are declared in RAM
bgPointerHi:  .byte 0             ; low byte first, high byte immediately after
counterLo:    .byte 0
counterHi:    .byte 0
x_player1:    .res 1  
y_player1:    .res 1
x_player2:    .res 1
y_player2:    .res 1
vblank:       .res 1


.segment "CODE"
reset:
  sei                             ; disable IRQs
  cld                             ; disable decimal mode
  ldx #$40
  stx $4017                       ; disable APU frame IRQ
  ldx #$FF
  txs                             ; Set up stack
  inx                             ; now X = 0
  stx $2000                       ; disable NMI
  stx $2001                       ; disable rendering
  stx $4010                       ; disable DMC IRQs

                                  ; Optional (omitted):
                                  ; Set up mapper and jmp to further init code here.

                                  ; If the user presses Reset during vblank, the PPU may reset
                                  ; with the vblank flag still true.  This has about a 1 in 13
                                  ; chance of happening on NTSC or 2 in 9 on PAL.  Clear the
                                  ; flag now so the @vblankwait1 loop sees an actual vblank.
  bit $2002

vblankwait1:                      ; First wait for vblank to make sure PPU is ready
  bit $2002
  bpl vblankwait1

clrmem:                           ; Set up RAM before waiting for the second vblank
  lda #$00
  sta $0000, x
  sta $0100, x
  sta $0300, x
  sta $0400, x
  sta $0500, x
  sta $0600, x
  sta $0700, x
  lda #$FE
  sta $0200, x                    ; Shouldn't be zeroed, since that would mean X position is 0
                                  ; Instead, we set the X poision to 0xFE, meaning it is offscreen
  inx
  bne clrmem

vblankwait2:                      ; Second wait for vblank, PPU is ready after this
  bit $2002
  bpl vblankwait2

  lda #<(background)
  sta bgPointerLo
  lda #>(background)
  sta bgPointerHi
  lda #$00
  sta counterLo                   ; put the loop counter into 16 bit variable
  lda #$04
  sta counterHi                   ; count = $0400 = 1KB, the whole screen at once including attributes
LoadBackground:
  lda $2002                       ; notifies cpu that we want to read/write the high value. (reset latch to high)
  lda #$20
  sta $2006                       ; first we write the upper byte of the ppu adress we want to ;write to in this case $3F00
  lda #$00
  sta $2006                       ; now we write the lower byte #$00
  ldy #$00                        ; unused
LoadBackgroundLoop:
  lda (bgPointerLo), y
  sta $2007

  ;;;;; INCREMENT 16-bit ;;;;;
  lda bgPointerLo
  clc
  adc #$01
  sta bgPointerLo
  lda bgPointerHi
  adc #$00
  sta bgPointerHi

  ;;;;; DECREMENT 16-bit ;;;;;;
  lda counterLo
  sec
  sbc #$01
  sta counterLo
  lda counterHi
  sbc #$00
  sta counterHi

  lda counterLo
  cmp #$00
  bne LoadBackgroundLoop
  lda counterHi
  cmp #$00
  bne LoadBackgroundLoop          ; if the loop counter isn't 0000, keep copying

LoadAttribute:
  lda $2002                       ; we don't need to reset the latch since it's already on high
  lda #$23
  sta $2006                       ; first we write the upper byte of the ppu adress we want to write to
  lda #$C0
  sta $2006                       ; now we write the lower byte #$00
  ldx #$00
LoadAttributeLoop:
  lda attribute, x
  sta $2007
  inx
  cpx #$8
  bne LoadAttributeLoop

LoadPalettes:
  lda $2002                       ; we don't need to reset the latch since it's already on high
  lda #$3F
  sta $2006                       ; first we write the upper byte of the ppu adress we want to write to
  lda #$00
  sta $2006                       ; now we write the lower byte #$00
  ldx #$00
LoadPalettesLoop:
  lda palette, x
  sta $2007
  inx
  cpx #$20
  bne LoadPalettesLoop

  lda #%10010000                  ; enable NMI, sprites from Pattern 0, background from Pattern 1
  sta $2000

  lda #%00011110                  ; enable sprites and background
  sta $2001

mainLoop:
  lda #$00
  sta vblank                      ; reset vblank lock

  lda #$01
  sta $4016                       ; poll input
  sta $4017                       
  lda #$00
  sta $4016                       ; stop polling input
  sta $4017                       

  ;player 1
  lda $4016                       ; A
  lda $4016                       ; B
  lda $4016                       ; Select
  lda $4016                       ; Start
  lda $4016                       ; Up
  and #%00000001
  beq skipUp
  dec y_player1
skipUp:
  lda $4016                       ; Down
  and #%00000001
  beq skipDown
  inc y_player1
skipDown:
  lda $4016                       ; Left
  and #%00000001
  beq skipLeft
  dec x_player1
skipLeft:
  lda $4016                       ; Right
  and #%00000001
  beq skipRight
  inc x_player1
skipRight:

  ;player 2
  lda $4017                       ; A
  lda $4017                       ; B
  lda $4017                       ; Select
  lda $4017                       ; Start
  lda $4017                       ; Up
  and #%00000001
  beq skipUp2
  dec y_player2
skipUp2:
  lda $4017                       ; Down
  and #%00000001
  beq skipDown2
  inc y_player2
skipDown2:
  lda $4017                       ; Left
  and #%00000001
  beq skipLeft2
  dec x_player2
skipLeft2:
  lda $4017                       ; Right
  and #%00000001
  beq skipRight2
  inc x_player2
skipRight2:
  
  ; player 1
  lda y_player1
  sta $0200                       ; Y
  lda x_player1
  sta $0203                       ; X
  lda #$00
  sta $0201                       ; tile number = 1
  lda #$00
  sta $0202                       ; color = 0, no flipping

  ; player 2
  lda y_player2
  sta $0204                       ; Y
  lda x_player2
  sta $0207                       ; X
  lda #$01
  sta $0205                       ; tile number = 1
  lda #$00
  sta $0206                       ; color = 0, no flipping

:
  lda vblank
  beq :-
  jmp mainLoop                    ; jump back to Forever, infinite loop

;;;;;;;;;;;;;;

nmi:                              ; VBLANK interrupt
  lda #$00
  sta $2003                       ; Why are the high and low so far apart????
  lda #$02
  sta $4014                       ; start dma

  lda #$00                        ; Scroll position!! This is needed because using PPUADDR overwrites PPUSCROLL!!
  sta $2005
  sta $2005
  lda #$01
  sta vblank
  rti

irq:
  rti

;;;;;;;;;;;;;;

.segment "GFX_DATA"
.include "gfx.s"

;;;;;;;;;;;;;;

.segment "CHARS"
.incbin "gfx/test.chr"
;.incbin "gfx/mario.chr"               ; includes 8KB graphics file from SMB1

