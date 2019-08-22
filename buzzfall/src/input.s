input_player_1:
  lda #$01
  sta $4016                       ; poll input
  lda #$00
  sta $4016                       ; stop polling input

  ;player 1
  lda $4016                       ; A
  lda $4016                       ; B
  lda $4016                       ; Select
  lda $4016                       ; Start
  lda $4016                       ; Up

  and #%00000001
  beq :+
  dec y_player1
  lda #<(y_player1)
  sta check_collision_y_addrs
  lda #>(y_player1)
  sta check_collision_y_addrs+1
  lda #<(x_player1)
  sta check_collision_x_addrs
  lda #>(x_player1)
  sta check_collision_x_addrs+1
  lda #1
  sta check_collision_dir
  jsr check_collision_segmented
:
  lda $4016                       ; Down
  and #%00000001
  beq :+
  inc y_player1
  lda #<(y_player1)
  sta check_collision_y_addrs
  lda #>(y_player1)
  sta check_collision_y_addrs+1
  lda #<(x_player1)
  sta check_collision_x_addrs
  lda #>(x_player1)
  sta check_collision_x_addrs+1
  lda #1
  sta check_collision_dir
  jsr check_collision_segmented
:
  lda $4016                       ; Left
  and #%00000001
  beq :+
  dec x_player1
  lda #<(y_player1)
  sta check_collision_y_addrs
  lda #>(y_player1)
  sta check_collision_y_addrs+1
  lda #<(x_player1)
  sta check_collision_x_addrs
  lda #>(x_player1)
  sta check_collision_x_addrs+1
  lda #0
  sta check_collision_dir
  jsr check_collision_segmented
:
  lda $4016                       ; Right
  and #%00000001
  beq :+
  inc x_player1
  lda #<(y_player1)
  sta check_collision_y_addrs
  lda #>(y_player1)
  sta check_collision_y_addrs+1
  lda #<(x_player1)
  sta check_collision_x_addrs
  lda #>(x_player1)
  sta check_collision_x_addrs+1
  lda #0
  sta check_collision_dir
  jsr check_collision_segmented
:
  rts

input_player_2:
  lda #$01
  sta $4017                       ; poll input
  lda #$00
  sta $4017                       ; stop polling input

  ;player 1
  lda $4017                       ; A
  lda $4017                       ; B
  lda $4017                       ; Select
  lda $4017                       ; Start
  lda $4017                       ; Up

  and #%00000001
  beq :+
  dec y_player2
  lda #<(y_player2)
  sta check_collision_y_addrs
  lda #>(y_player2)
  sta check_collision_y_addrs+1
  lda #<(x_player2)
  sta check_collision_x_addrs
  lda #>(x_player2)
  sta check_collision_x_addrs+1
  lda #1
  sta check_collision_dir
  jsr check_collision_segmented
:
  lda $4017                       ; Down
  and #%00000001
  beq :+
  inc y_player2
  lda #<(y_player2)
  sta check_collision_y_addrs
  lda #>(y_player2)
  sta check_collision_y_addrs+1
  lda #<(x_player2)
  sta check_collision_x_addrs
  lda #>(x_player2)
  sta check_collision_x_addrs+1
  lda #1
  sta check_collision_dir
  jsr check_collision_segmented
:
  lda $4017                       ; Left
  and #%00000001
  beq :+
  dec x_player2
  lda #<(y_player2)
  sta check_collision_y_addrs
  lda #>(y_player2)
  sta check_collision_y_addrs+1
  lda #<(x_player2)
  sta check_collision_x_addrs
  lda #>(x_player2)
  sta check_collision_x_addrs+1
  lda #0
  sta check_collision_dir
  jsr check_collision_segmented
:
  lda $4017                       ; Right
  and #%00000001
  beq :+
  inc x_player2
  lda #<(y_player2)
  sta check_collision_y_addrs
  lda #>(y_player2)
  sta check_collision_y_addrs+1
  lda #<(x_player2)
  sta check_collision_x_addrs
  lda #>(x_player2)
  sta check_collision_x_addrs+1
  lda #0
  sta check_collision_dir
  jsr check_collision_segmented
:
  rts

check_collision_ver:
  asl
  asl
  asl
  asl
  asl
  sta add_buffer
@check_up:
  ldy #0
  lda (check_collision_x_addrs), y
  clc
  adc #7
  lsr
  lsr
  lsr
  clc
  adc add_buffer
  tay
  lda (check_collision_bg_addrs), y
  beq :+

  ldy #0
  lda (check_collision_x_addrs), y
  lsr
  lsr
  lsr
  clc
  adc add_buffer
  tay
  lda (check_collision_bg_addrs), y
  beq :+

  jmp @check_down
:
  ldy #0
  lda (check_collision_y_addrs), y
  and #%111
  sta add_buffer2
  lda #7
  sec
  sbc add_buffer2
  clc
  adc (check_collision_y_addrs), y
  sta (check_collision_y_addrs), y
  jmp @check_collision_end
@check_down:
  ldy #0
  lda (check_collision_x_addrs), y
  clc
  adc #7
  lsr
  lsr
  lsr
  clc
  adc add_buffer
  clc
  adc #32
  tay
  lda (check_collision_bg_addrs), y
  beq :+

  ldy #0
  lda (check_collision_x_addrs), y
  lsr
  lsr
  lsr
  clc
  adc add_buffer
  clc
  adc #32
  tay
  lda (check_collision_bg_addrs), y
  beq :+
  jmp @check_collision_end
:
  ldy #0
  lda (check_collision_y_addrs), y
  and #%111
  sta add_buffer2
  lda (check_collision_y_addrs), y
  clc
  sbc add_buffer2
  sta (check_collision_y_addrs), y
@check_collision_end:
  rts

check_collision_hor:
  asl
  asl
  asl
  asl
  asl
  sta add_buffer
@check_left:
  ldy #0
  lda (check_collision_x_addrs), y
  lsr
  lsr
  lsr
  clc
  adc add_buffer
  tay
  lda (check_collision_bg_addrs), y
  beq :+

  ldy #0
  lda (check_collision_x_addrs), y
  lsr
  lsr
  lsr
  clc
  adc add_buffer
  clc
  adc #32
  tay
  lda (check_collision_bg_addrs), y
  beq :+

  jmp @check_right
:
  ldy #0
  lda (check_collision_x_addrs), y
  and #%111
  sta add_buffer2
  lda #7
  sec
  sbc add_buffer2
  sec
  adc (check_collision_x_addrs), y
  sta (check_collision_x_addrs), y
  jmp @check_collision_end
@check_right:
  ldy #0
  lda (check_collision_x_addrs), y
  clc
  adc #7
  lsr
  lsr
  lsr
  clc
  adc add_buffer
  tay
  lda (check_collision_bg_addrs), y
  beq :+

  ldy #0
  lda (check_collision_x_addrs), y
  clc
  adc #7
  lsr
  lsr
  lsr
  clc
  adc add_buffer
  clc
  adc #32
  tay
  lda (check_collision_bg_addrs), y
  beq :+

  jmp @check_collision_end
:
  ldy #0
  lda (check_collision_x_addrs), y
  and #%111
  sta add_buffer2
  lda (check_collision_x_addrs), y
  sec
  sbc add_buffer2
  sta (check_collision_x_addrs), y

@check_collision_end:
  rts


check_collision_segmented:
  ldy #0
  lda (check_collision_y_addrs), y
  lsr
  lsr
  lsr
@screen_segment_0:
  cmp #6
  bpl @screen_segment_1
  ;background
  ldx #<(background)
  stx check_collision_bg_addrs
  ldx #>(background)
  stx check_collision_bg_addrs+1

  ldx check_collision_dir
  beq :+
  jsr check_collision_ver
  jmp @check_collision_segmented_end
  :
  jsr check_collision_hor
  jmp @check_collision_segmented_end

@screen_segment_1:
  cmp #12
  bpl @screen_segment_2
  ;background1
  ldx #<(background1)
  stx check_collision_bg_addrs
  ldx #>(background1)
  stx check_collision_bg_addrs+1

  sec
  sbc #6
  ldx check_collision_dir
  beq :+
  jsr check_collision_ver
  jmp @check_collision_segmented_end
  :
  jsr check_collision_hor
  jmp @check_collision_segmented_end

@screen_segment_2:
  cmp #18
  bpl @screen_segment_3
  ;background2
  ldx #<(background2)
  stx check_collision_bg_addrs
  ldx #>(background2)
  stx check_collision_bg_addrs+1

  sec
  sbc #12
  ldx check_collision_dir
  beq :+
  jsr check_collision_ver
  jmp @check_collision_segmented_end
  :
  jsr check_collision_hor
  jmp @check_collision_segmented_end

@screen_segment_3:
  cmp #24
  bpl @screen_segment_4
  ;background3
  ldx #<(background3)
  stx check_collision_bg_addrs
  ldx #>(background3)
  stx check_collision_bg_addrs+1

  sec
  sbc #18
  ldx check_collision_dir
  beq :+
  jsr check_collision_ver
  jmp @check_collision_segmented_end
  :
  jsr check_collision_hor
  jmp @check_collision_segmented_end

@screen_segment_4:
  ;background4
  ldx #<(background4)
  stx check_collision_bg_addrs
  ldx #>(background4)
  stx check_collision_bg_addrs+1

  sec
  sbc #24
  ldx check_collision_dir
  beq :+
  jsr check_collision_ver
  jmp @check_collision_segmented_end
  :
  jsr check_collision_hor
@check_collision_segmented_end:
  rts
