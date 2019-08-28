JUMP_BOOST = %0111
ANIMATION_TIMER = 8

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
.segment "ZEROPAGE"              ; is this really supposed to be used like this? what about rs = 1?
bgPointerLo:  .res 1             ; pointer variables are declared in RAM
bgPointerHi:  .res 1             ; low byte first, high byte immediately after

counterLo:    .res 1
counterHi:    .res 1

vblank:       .res 1            ; vblank flag (set by nmi)

add_buffer:   .res 1
add_buffer2:  .res 1

dummy:        .res 1

; Player 1 variables
direction1:   .res 1            ; direction flag (0 -> left / 1 -> right / 2 -> up / 3->down)
x_player1:    .res 1
y_player1:    .res 1
v_player1:    .res 1 ;Fixed-point -> 5.3
walking1:     .res 1 ; 0 -> not walking / 1 -> walking
arrows_player1: .res 1

jump_counter1:  .res 1 ;Fixed-point -> 5.3
jump_disabled1: .res 1 ;0 -> can jump / 1 -> can't jump
walljump_cooldown1: .res 1
walljump_disabled1: .res 1 ;0 -> can jump / 1 -> can't jump
animation_cur_tile1: .res 1
;

; Player 2 variables
direction2:   .res 1            ; direction flag (0 -> left / 1 -> right / 2 -> up / 3->down)
x_player2:    .res 1
y_player2:    .res 1
v_player2:    .res 1 ;Fixed-point -> 5.3
walking2:     .res 1 ; 0 -> not walking / 1 -> walking
arrows_player2: .res 1

jump_counter2:  .res 1 ;Fixed-point -> 5.3
jump_disabled2: .res 1 ;0 -> can jump / 1 -> can't jump
walljump_cooldown2: .res 1
walljump_disabled2: .res 1 ;0 -> can jump / 1 -> can't jump
animation_cur_tile2: .res 1
;

; Arrow 1 variables
arrow_size:  .res 1        ; 1 -> arrow 1 on screen / 0 -> no arrow
x_arrow:     .res 10
y_arrow:     .res 10
d_arrow:     .res 10
owner_arrow: .res 10       ; 0 -> no one / 1 -> player 1 / 2-> player 2
;

; check_collision args
check_collision_y_addrs:  .res 2
check_collision_x_addrs:  .res 2
check_collision_v_addrs:  .res 2
check_collision_j_addrs:  .res 2
check_collision_wj_addrs: .res 2
check_collision_dir:      .res 1 ; 1-> VERTICAL, 0-> HORIZONTAL

check_collision_bg_addrs: .res 2
;

; Animation variables
animation_timer:          .res 1
;

.segment "BSS"
shadow_oam: .res 256

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
  sta shadow_oam, x               ; Shouldn't be zeroed, since that would mean X position is 0
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

; Put players inside the walls
  lda #20
  sta x_player1
  sta y_player1
  lda #30
  sta x_player2
  sta y_player2

; Initialize animations
  lda #ANIMATION_TIMER
  sta animation_timer
  lda #0
  sta animation_cur_tile1
  sta animation_cur_tile2

; Initialize players' shadow_oam
  lda #0
  sta shadow_oam+2                ; color = 0, no flipping
  lda #1
  sta shadow_oam+6                ; color = 1, no flipping

  lda #0
  sta arrow_size

; Initialize payers arrow count
  lda #2
  sta arrows_player1
  sta arrows_player2

mainLoop:
  lda #$00
  sta vblank                      ; reset vblank lock

  jsr input_player_1
  jsr input_player_2

  ; player1 gravity
  inc v_player1

  ; player1 jump
  lda jump_counter1
  beq :+
  dec jump_counter1
  lda v_player1
  sec
  sbc jump_counter1
  sta v_player1
:

  ; player1 apply velocity
  lda v_player1
  jsr fixed2int
  clc
  adc y_player1
  sta y_player1

  ; player1 apply walljump cooldown
  lda walljump_cooldown1
  beq :+
  dec walljump_cooldown1
:

  ; player1 collision
  lda #<(y_player1)
  sta check_collision_y_addrs
  lda #>(y_player1)
  sta check_collision_y_addrs+1
  lda #<(x_player1)
  sta check_collision_x_addrs
  lda #>(x_player1)
  sta check_collision_x_addrs+1
  lda #<(v_player1)
  sta check_collision_v_addrs
  lda #>(v_player1)
  sta check_collision_v_addrs+1
  lda #<(walljump_disabled1)
  sta check_collision_wj_addrs
  lda #>(walljump_disabled1)
  sta check_collision_wj_addrs+1
  lda #<(jump_disabled1)
  sta check_collision_j_addrs
  lda #>(jump_disabled1)
  sta check_collision_j_addrs+1
  lda #$1
  sta check_collision_dir
  jsr check_collision_segmented

  ; player2 gravity
  inc v_player2

  ; player2 jump
  lda jump_counter2
  beq :+
  dec jump_counter2
  lda v_player2
  sec
  sbc jump_counter2
  sta v_player2
:

  ; player2 apply velocity
  lda v_player2
  jsr fixed2int
  clc
  adc y_player2
  sta y_player2

  ; player2 apply walljump cooldown
  lda walljump_cooldown2
  beq :+
  dec walljump_cooldown2
:
  ; player2 collision
  lda #<(y_player2)
  sta check_collision_y_addrs
  lda #>(y_player2)
  sta check_collision_y_addrs+1
  lda #<(x_player2)
  sta check_collision_x_addrs
  lda #>(x_player2)
  sta check_collision_x_addrs+1
  lda #<(v_player2)
  sta check_collision_v_addrs
  lda #>(v_player2)
  sta check_collision_v_addrs+1
  lda #<(walljump_disabled2)
  sta check_collision_wj_addrs
  lda #>(walljump_disabled2)
  sta check_collision_wj_addrs+1
  lda #<(jump_disabled2)
  sta check_collision_j_addrs
  lda #>(jump_disabled2)
  sta check_collision_j_addrs+1
  lda #$1
  sta check_collision_dir
  jsr check_collision_segmented


  ;;;;; UPDATE ANIMATIONS ;;;;;

  ; This code changes the way the character is animated
  ; when leaving a wall after a walljump is performed
  ; Currently we decided to leave it commented since
  ; the current animation works better without it.

;  lda walljump_disabled1
;  beq @skipJumpAnimation1         ; if player is in the air
;  lda jump_disabled1
;  beq @skipJumpAnimation1
;  lda animation_cur_tile1
;  lsr
;  lsr
;  lsr
;  cmp #3
;  bne @skipJumpAnimation1         ; and he is in walljump animation
;  lda #16
;  sta animation_cur_tile1
;@skipJumpAnimation1:
;
;  lda walljump_disabled2
;  beq @skipJumpAnimation2         ; if player is in the air
;  lda jump_disabled2
;  beq @skipJumpAnimation2
;  lda animation_cur_tile2
;  lsr
;  lsr
;  lsr
;  cmp #3
;  bne @skipJumpAnimation2         ; and he is in walljump animation
;  lda #16
;  sta animation_cur_tile2
;@skipJumpAnimation2:

  lda walking1
  bne @skipIdleAnimation1         ; if player isn't walking
  lda jump_disabled1
  bne @skipIdleAnimation1         ; and jump is enabled
  lda animation_cur_tile1
  lsr
  lsr
  lsr
  cmp #0
  beq @skipIdleAnimation1         ; and he is in another animation
  cmp #4                          ; that isn't shooting animation ...
  bne :+
  lda animation_cur_tile1
  and #%111
  cmp #4                          ; ... after the fourth frame
  bmi @skipIdleAnimation1
:
  lda #0
  sta animation_cur_tile1
@skipIdleAnimation1:

  lda walking2
  bne @skipIdleAnimation2         ; if player isn't walking
  lda jump_disabled2
  bne @skipIdleAnimation2         ; and jump is enabled
  lda animation_cur_tile2
  lsr
  lsr
  lsr
  cmp #0
  beq @skipIdleAnimation2         ; and he is in another animation
  cmp #4                          ; that isn't shooting animation ...
  bne :+
  lda animation_cur_tile2
  and #%111
  cmp #4                          ; ... after the fourth frame
  bmi @skipWalkAnimation2
:
  lda #0
  sta animation_cur_tile2
@skipIdleAnimation2:

  lda walking1
  beq @skipWalkAnimation1         ; if player is walking
  lda jump_disabled1
  bne @skipWalkAnimation1         ; and jump is enabled
  lda animation_cur_tile1
  lsr
  lsr
  lsr
  cmp #1
  beq @skipWalkAnimation1         ; and he is in another animation
  cmp #4                          ; that isn't shooting animation ...
  bne :+
  lda animation_cur_tile1
  and #%111
  cmp #4                          ; ... after the fourth frame
  bmi @skipWalkAnimation1
:
  lda #8
  sta animation_cur_tile1
@skipWalkAnimation1:

  lda walking2
  beq @skipWalkAnimation2         ; if player is walking
  lda jump_disabled2
  bne @skipWalkAnimation2         ; and jump is enabled
  lda animation_cur_tile2
  lsr
  lsr
  lsr
  cmp #1
  beq @skipWalkAnimation2         ; and he is in another animation
  cmp #4                          ; that isn't shooting animation ...
  bne :+
  lda animation_cur_tile2
  and #%111
  cmp #4                          ; ... after the fourth frame
  bmi @skipWalkAnimation2
:
  lda #8
  sta animation_cur_tile2
@skipWalkAnimation2:

  dec animation_timer             ; decrement animation timer
  lda animation_timer
  bne @skipChangeAnimTile         ; did animation timer hit 0?
  
                                  ; change animation frame for player 1
  inc animation_cur_tile1         ; move to the next animation tile
  lda animation_cur_tile1
  and #%111                       ; a = animation_cur_tile MOD 8
  tax                             ; X is the animation frame (each animation has 6 frames)
  cmp #06                         ; a == 6 ? (go back to 0 if its a looping anim, otherwise go back to 5) : (skip)
  bne @changeAnimationFrame1
  lda animation_cur_tile1         ; check if it is a looping animation
  lsr
  lsr
  lsr
  cmp #2
  bmi @loopingAnim1
  ldx #5
  jmp @changeAnimationFrame1
@loopingAnim1:
  ldx #0
@changeAnimationFrame1:
  stx add_buffer
  lda animation_cur_tile1
  and #%11111000
  clc
  adc add_buffer
  sta animation_cur_tile1
  lda #ANIMATION_TIMER
  sta animation_timer             ; reset animation timer

                                  ; change animation frame for player 2
  inc animation_cur_tile2         ; move to the next animation tile
  lda animation_cur_tile2
  and #%111                       ; a = animation_cur_tile MOD 8
  tax                             ; X is the animation frame (each animation has 6 frames)
  cmp #06                         ; a == 6 ? (go back to 0 if its a looping anim, otherwise go back to 5) : (skip)
  bne @changeAnimationFrame2
  lda animation_cur_tile2         ; check if it is a looping animation
  lsr
  lsr
  lsr
  cmp #2
  bmi @loopingAnim2
  ldx #5
  jmp @changeAnimationFrame2
@loopingAnim2:
  ldx #0
@changeAnimationFrame2:
  stx add_buffer
  lda animation_cur_tile2
  and #%11111000
  clc
  adc add_buffer
  sta animation_cur_tile2
  lda #ANIMATION_TIMER
  sta animation_timer             ; reset animation timer
@skipChangeAnimTile:


  ;;;;; UPDATE SHADOW OAM ;;;;;

  ; player 1
  lda y_player1
  sta shadow_oam                  ; Y
  lda x_player1
  sta shadow_oam+3                ; X
  lda animation_cur_tile1
  sta shadow_oam+1                ; tile number = 0

  lda direction1
  bne :+                          ; Looking to the left?
  lda #%00000000
  sta shadow_oam+2                ; color = 0, no flipping
:
  lda direction1
  cmp #1
  bne :+                          ; Looking to the right?
  lda #%01000000
  sta shadow_oam+2                ; color = 0, flipping
:                                 ; Notice that because the color doesn't change we
                                  ; don't need to write to this byte every frame


  ; player 2
  lda y_player2
  sta shadow_oam+4                ; Y
  lda x_player2
  sta shadow_oam+7                ; X
  lda animation_cur_tile2
  sta shadow_oam+5                ; tile number = 1

  lda direction2
  bne :+                          ; Looking to the left?
  lda #%00000001
  sta shadow_oam+6                ; color = 0, no flipping
:
  lda direction2
  cmp #1
  bne :+                          ; Looking to the right?
  lda #%01000001
  sta shadow_oam+6                ; color = 0, flipping
:                                 ; Notice that because the color doesn't change we
                                  ; don't need to write to this byte every frame

  ; arrow 1 velocity
  ldx arrow_size
@arrow_loop:
  cpx #0
  bpl :+
  jmp @arrow_loop_break
:
  lda d_arrow, x
@left:                            ; check the direction of the arrow                  
  cmp #$0
  bne @right
  dec x_arrow, x
  dec x_arrow, x
  ldy #0
  jmp :+
@right:
  cmp #$1
  bne @up
  inc x_arrow, x
  inc x_arrow, x
  ldy #0
  jmp :+
@up:
  cmp #$2
  bne @down
  dec y_arrow, x
  dec y_arrow, x
  ldy #1
  jmp :+
@down:
  cmp #$3
  inc y_arrow, x
  inc y_arrow, x
  ldy #1
:
  txa
  pha

  stx add_buffer
  lda #<(y_arrow)
  clc
  adc add_buffer
  sta check_collision_y_addrs
  lda #>(y_arrow)
  sta check_collision_y_addrs+1
  lda #<(x_arrow)
  clc
  adc add_buffer
  sta check_collision_x_addrs
  lda #>(x_arrow)
  sta check_collision_x_addrs+1
  lda #<(dummy)
  sta check_collision_v_addrs
  sta check_collision_wj_addrs
  sta check_collision_j_addrs
  lda #>(dummy)
  sta check_collision_v_addrs+1
  sta check_collision_wj_addrs+1
  sta check_collision_j_addrs+1
  sty check_collision_dir
  jsr check_collision_segmented

  pla
  tax

  ; arrow player 1
  txa
  asl
  asl
  clc
  adc #8
  clc
  adc #<(shadow_oam)
  sta add_buffer
  lda #>(shadow_oam)
  sta add_buffer2

  ldy #0
  lda y_arrow, x
  sta (add_buffer), y             ; Y
  ldy #1
  lda #64
  sta (add_buffer), y             ; tile number = 2
  ldy #2
  lda #%10
  sta (add_buffer), y             ; color = 2, no flipping
  ldy #3
  lda x_arrow, x
  sta (add_buffer), y             ; X

  dex
  jmp @arrow_loop
@arrow_loop_break:
:
  lda vblank
  beq :-

  jmp mainLoop                    ; jump back to Forever, infinite loop

;;;;;;;;;;;;;;

; a -> number to be converted
fixed2int:
  cmp #0
  bpl :+
  sec
  ror
  sec
  ror
  sec
  ror
  jmp @fixed2int_end
:
  clc
  ror
  clc
  ror
  clc
  ror
@fixed2int_end:
  rts

; a -> number to be converted
int2fixed:
  asl
  asl
  asl

;;;;;;;;;;;;;;

.include "input.s"

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
.incbin "gfx/arqueiro_states.chr"
.incbin "gfx/flecha.chr"
.res 3056
.incbin "gfx/bg.chr"
