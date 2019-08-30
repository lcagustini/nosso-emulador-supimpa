test_sprite_collision:
  lda test_sprite_x1
  clc
  adc #7
  cmp test_sprite_x2
  php
  pla
  and #%100000
  sta add_buffer
  lda test_sprite_x2
  clc
  adc #7
  cmp test_sprite_x1
  php
  pla
  and #%100000
  ora add_buffer
  sta add_buffer
  lda test_sprite_y1
  clc
  adc #7
  cmp test_sprite_y2
  php
  pla
  and #%100000
  ora add_buffer
  sta add_buffer
  lda test_sprite_y2
  clc
  adc #7
  cmp test_sprite_y1
  php
  pla
  and #%100000
  ora add_buffer
  rts 
