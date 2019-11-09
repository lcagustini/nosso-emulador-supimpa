// PPUCTRL
#define GET_BASE_NAMETABLE_ID() (ppu.ctrl & 0b11)
#define SET_BASE_NAMETABLE_ID(a) (ppu.ctrl = (ppu.ctrl & (~0b11)) | (a & 0b11))
#define NAMETABLE_ID_TO_ADDRS(a) ( \
    (a) == 0 ? 0x2000 : ( \
    (a) == 1 ? 0x2400 : ( \
    (a) == 2 ? 0x2800 : 0x2C00 )) \
  )

#define GET_VRAM_PPU_INCREMENT() (ppu.ctrl & BIT2)
#define SET_VRAM_PPU_INCREMENT(a) (ppu.ctrl = (ppu.ctrl & (~BIT2)) | (a & BIT0))

#define GET_SPRITE_PATTERN_TABLE_ID() (ppu.ctrl & BIT3)
#define SET_SPRITE_PATTERN_TABLE_ID(a) (ppu.ctrl = (ppu.ctrl & (~BIT3)) | (a & BIT0))

#define GET_BACKGROUND_PATTERN_TABLE_ID() (ppu.ctrl & BIT4)
#define SET_BACKGROUND_PATTERN_TABLE_ID(a) (ppu.ctrl = (ppu.ctrl & (~BIT4)) | (a & BIT0))
#define PATTERN_ID_TO_ADDRS(a) (a == 0 ? 0 : 0x1000)

#define GET_SPRITE_SIZE() (ppu.ctrl & BIT5)
#define SET_SPRITE_SIZE(a) (ppu.ctrl = (ppu.ctrl & (~BIT5)) | (a & BIT0))

#define GET_PPU_MASTER_SELECT() (ppu.ctrl & BIT6)
#define SET_PPU_MASTER_SELECT(a) (ppu.ctrl = (ppu.ctrl & (~BIT6) | (a & BIT0)))

#define GET_NMI_ENABLE() (ppu.ctrl & BIT7)
#define SET_NMI_ENABLE(a) (ppu.ctrl = (ppu.ctrl & (~BIT7)) | (a & BIT0))

// PPUMASK
#define GET_GRAYSCALE_ENABLE() (ppu.mask & BIT0)
#define SET_GRAYSCALE_ENABLE(a) (ppu.mask = (ppu.mask & (~BIT0)) | (a & BIT0))

#define GET_BACKGROUND_MASK() (ppu.mask & BIT1)
#define SET_BACKGROUND_MASK(a) (ppu.mask = (ppu.mask & (~BIT1)) | (a & BIT0))

#define GET_SPRITE_MASK() (ppu.mask & BIT2)
#define SET_SPRITE_MASK(a) (ppu.mask = (ppu.mask & (~BIT2)) | (a & BIT0))

#define GET_BACKGROUND_ENABLE() (ppu.mask & BIT3)
#define SET_BACKGROUND_ENABLE(a) (ppu.mask = (ppu.mask & (~BIT3)) | (a & BIT0))

#define GET_SPRITE_ENABLE() (ppu.mask & BIT4)
#define SET_SPRITE_ENABLE(a) (ppu.mask = (ppu.mask & (~BIT4)) | (a & BIT0))

#define GET_RED_FILTER() (ppu.mask & BIT5)
#define SET_RED_FILTER(a) (ppu.mask = (ppu.mask & (~BIT5)) | (a & BIT0))

#define GET_GREEN_FILTER() (ppu.mask & BIT6)
#define SET_GREEN_FILTER(a) (ppu.mask = (ppu.mask & (~BIT6)) | (a & BIT0))

#define GET_BLUE_FILTER() (ppu.mask & BIT7)
#define SET_BLUE_FILTER(a) (ppu.mask = (ppu.mask & (~BIT7)) | (a & BIT0))

// OAM
#define GET_SPRITE_Y(a) (ppu.oam.data[a*4+0])
#define GET_SPRITE_TILE_NUMBER(a) (ppu.oam.data[a*4+1]) // does not work for 8x16 sprites
#define GET_SPRITE_PALETTE(a) ((ppu.oam.data[a*4+2]) & (BIT0|BIT1))
#define GET_SPRITE_PRIORITY(a) (((ppu.oam.data[a*4+2]) & (BIT5)) >> 5)
#define GET_SPRITE_FLIP_H(a) (((ppu.oam.data[a*4+2]) & (BIT6)) >> 6)
#define GET_SPRITE_FLIP_V(a) (((ppu.oam.data[a*4+2]) & (BIT7)) >> 7)
#define GET_SPRITE_X(a) ((ppu.oam.data[a*4+3]))

// PPUSTATUS
// TODO: Implement the 5 least significant bits
#define GET_SPRITE_OVERFLOW() (ppu.status & BIT5)
#define GET_SPRITE_0_HIT() (ppu.status & BIT6)
#define GET_VBLANK_START() (ppu.status & BIT7)

#define GET_ATTRIBUTETABLE_ADDRS(a) ( \
    a == 0 ? 0x23C0 : ( \
    a == 1 ? 0x27C0 : ( \
    a == 2 ? 0x2BC0 : 0x2FC0 )) \
  )

#define BACKGROUND_PALETTE_ID_TO_ADDRS(a) ( \
    a == 0 ? 0x3F00 : ( \
    a == 1 ? 0x3F04 : ( \
    a == 2 ? 0x3F08 : 0x3F0C )) \
  )

#define SPRITE_PALETTE_ID_TO_ADDRS(a) ( \
    a == 0 ? 0x3F10 : ( \
    a == 1 ? 0x3F14 : ( \
    a == 2 ? 0x3F18 : 0x3F1C )) \
  )

void oamDMA(uint8_t hibyte) {
  uint16_t addrs = (hibyte << 8);

  for (int i = 0; ; i++, ppu.oam.addrs++) {
    ppu.oam.data[ppu.oam.addrs] = readCPUByte(addrs + i, true);
    if (ppu.oam.addrs == 0xFF) {
      ppu.oam.addrs = 0;
      break;
    }
  }

  if (cpu.clock_cycles % 2) cpu.clock_cycles += 1;
  cpu.clock_cycles += 513;
}

void decodeTile(uint8_t tile[16], uint8_t decoded_tile[64]) {
  for (int byte = 0; byte < 8; byte++) {
    for (int bit = 0; bit < 8; bit++) {
      decoded_tile[byte*8 + (7-bit)] = ((tile[byte] & (1 << bit)) | ((tile[byte + 8] & (1 << bit)) << 1)) >> bit;
    }
  }
}

void backgroundPaletteIndexAt(uint16_t x, uint16_t y, uint16_t *addrs_palette, uint8_t *pixel_palette) {
  uint8_t nametable_id = GET_BASE_NAMETABLE_ID();
  x += nametable_id & 1 ? 256 : 0;
  y += nametable_id & 2 ? 240 : 0;

  uint8_t tile_x = (x/8)%64;
  uint8_t tile_y = (y/8)%60;

  nametable_id = 0;
  if (tile_x >= 32 && tile_y < 30) {
    nametable_id = 1;
  }
  else if (tile_x < 32 && tile_y >= 30) {
    nametable_id = 2;
  }
  else if (tile_x >= 32 && tile_y >= 30) {
    nametable_id = 3;
  }
  uint16_t addrs_nametable = NAMETABLE_ID_TO_ADDRS(nametable_id);

  tile_x %= 32;
  tile_y %= 30;

  uint8_t pattern_id = readPPUByte(addrs_nametable + 32*tile_y + tile_x, true);
  uint16_t addrs_patterntable = PATTERN_ID_TO_ADDRS(GET_BACKGROUND_PATTERN_TABLE_ID());
  uint8_t tile[16];
  uint8_t decoded_tile[64];

  for (int i = 0; i < 16; i++) {
    tile[i] = readPPUByte(addrs_patterntable + 16*pattern_id + i, true);
  }
  decodeTile(tile, decoded_tile);

  uint16_t addrs_attributetable = GET_ATTRIBUTETABLE_ADDRS(nametable_id);
  uint8_t attribute_tile_x = tile_x/4;
  uint8_t attribute_tile_y = tile_y/4;
  uint8_t attribute_tile = readPPUByte(addrs_attributetable + 8*attribute_tile_y + attribute_tile_x, true);
  tile_x = tile_x % 4;
  tile_y = tile_y % 4;

  uint8_t palette_id;

  if (tile_x < 2 && tile_y < 2) palette_id = attribute_tile & 0b11;
  else if (tile_x >= 2 && tile_y < 2) palette_id = (attribute_tile & 0b1100) >> 2;
  else if (tile_x < 2 && tile_y >= 2) palette_id = (attribute_tile & 0b110000) >> 4;
  else palette_id = (attribute_tile & 0b11000000) >> 6;

  *addrs_palette = BACKGROUND_PALETTE_ID_TO_ADDRS(palette_id);
  x = x % 8;
  y = y % 8;

  *pixel_palette = decoded_tile[y*8 + x];
}

uint8_t backgroundPaletteIndexToColor(uint16_t addrs_palette, uint8_t pixel_palette) {
  if (pixel_palette == 0) return readPPUByte(0x3F00, true);
  return readPPUByte(addrs_palette + pixel_palette, true);
}

priority_t spritePaletteIndexAt(uint8_t x, uint8_t y, uint16_t *addrs_palette, uint8_t *pixel_palette, uint8_t *sprite_id) {
  // TODO: 8x16 sprites
  for (int i = 0; i < 64; i++) {
    uint8_t sprite_x = GET_SPRITE_X(i);
    uint8_t sprite_y = GET_SPRITE_Y(i);
    uint8_t tile_number = GET_SPRITE_TILE_NUMBER(i);
    uint8_t palette = GET_SPRITE_PALETTE(i);
    uint8_t priority = GET_SPRITE_PRIORITY(i);
    uint8_t flip_h = GET_SPRITE_FLIP_H(i);
    uint8_t flip_v = GET_SPRITE_FLIP_V(i);

    if (x >= sprite_x && x < sprite_x + 8 &&
        y >= sprite_y && y < sprite_y + 8) {

      uint16_t addrs_patterntable = PATTERN_ID_TO_ADDRS(GET_SPRITE_PATTERN_TABLE_ID());
      uint8_t tile[16];
      uint8_t decoded_tile[64];

      for (int i = 0; i < 16; i++) {
        tile[i] = readPPUByte(addrs_patterntable + 16*tile_number + i, true);
      }
      decodeTile(tile, decoded_tile);

      *addrs_palette = SPRITE_PALETTE_ID_TO_ADDRS(palette);
      uint8_t tx = x - sprite_x;
      uint8_t ty = y - sprite_y;

      if (flip_v) ty = 7 - ty;
      if (flip_h) tx = 7 - tx;

      *pixel_palette = decoded_tile[ty*8 + tx];
      *sprite_id = i;

      if (!*pixel_palette) continue;

      return priority + 1; // based on sprite_priority enum
    }
  }

  return P_NO_SPRITE;
}

uint8_t spritePaletteIndexToColor(uint16_t addrs_palette, uint8_t pixel_palette) {
  return readPPUByte(addrs_palette + pixel_palette, true);
}

void drawTVScreenPixel(SDL_Surface *draw_surface) {
  uint16_t x = ppu.draw.x + ppu.scroll.x;
  uint16_t y = ppu.draw.y + ppu.scroll.y;

  uint32_t *pixels = draw_surface->pixels;
  uint16_t addrs_palette;
  uint8_t sprite_palette;
  uint8_t backgroud_palette;

  backgroundPaletteIndexAt(x, y, &addrs_palette, &backgroud_palette);
  uint8_t backgroud_color = backgroundPaletteIndexToColor(addrs_palette, backgroud_palette);

  uint8_t sprite_id = 1;
  priority_t sprite_priority = spritePaletteIndexAt(ppu.draw.x, ppu.draw.y, &addrs_palette, &sprite_palette, &sprite_id);
  uint8_t sprite_color = spritePaletteIndexToColor(addrs_palette, sprite_palette);

  if (sprite_priority && sprite_id == 0 && sprite_palette && backgroud_palette && ppu.draw.x != 255) ppu.status |= BIT6;

  if (sprite_priority == P_OVER_BG || (sprite_priority == P_UNDER_BG && !backgroud_palette)) {
    pixels[(ppu.draw.y-8)*draw_surface->w + ppu.draw.x] = nes_palette[sprite_color]; // ARGB
  }
  else {
    pixels[(ppu.draw.y-8)*draw_surface->w + ppu.draw.x] = nes_palette[backgroud_color]; // ARGB
  }
}

void vblank(SDL_Window *window, SDL_Surface *draw_surface, SDL_Surface *screen_surface) {
  // sleep remaining time if we're too fast
  // TODO: try for-based sleep
  {
    struct timeval t;
    gettimeofday(&t, NULL);
    long int diff_usec = 0;
    if (t.tv_sec > last_frame_time.tv_sec) { // this does not work for more than 1 second per frame
      assert(t.tv_sec == last_frame_time.tv_sec+1);
      diff_usec += t.tv_usec;
      diff_usec += 1000000 - last_frame_time.tv_usec;
    } else {
      assert(t.tv_sec == last_frame_time.tv_sec);
      diff_usec += t.tv_usec - last_frame_time.tv_usec;
    }
    last_frame_time = t;

    float target_usec_per_frame = (1/60.0f) * 1000000.0f;
    float sleep_time = target_usec_per_frame - (float) diff_usec;
    if (sleep_time > 0) {
      usleep(sleep_time);
    }
  }

  // draw frame
  SDL_BlitScaled(draw_surface, NULL, screen_surface, NULL);

#ifdef PPU_CHR_PRINT
  for (int tile = 0; tile < 3840; tile++) {
    int tx = 8*(tile % 64);
    int ty = 8*(tile / 64);
    uint32_t *pixels = screen_surface->pixels;

    for (int iy = 0; iy < 8; iy++) {
      for (int ix = 0; ix < 8; ix++) {
        int x = tx + ix;
        int y = ty + iy;

        uint16_t addrs_palette;
        uint8_t backgroud_palette;

        backgroundPaletteIndexAt(x, y, &addrs_palette, &backgroud_palette);
        uint8_t backgroud_color = backgroundPaletteIndexToColor(addrs_palette, backgroud_palette);

        pixels[y*screen_surface->w + x] = nes_palette[backgroud_color]; // ARGB
      }
    }
  }
#endif

  SDL_UpdateWindowSurface(window);
}

void draw(SDL_Window *window, SDL_Surface *draw_surface, SDL_Surface *screen_surface) {
  while (3*cpu.clock_cycles > ppu.clock_cycles) {
    if (ppu.draw.x < 256 && ppu.draw.y >= 8 && ppu.draw.y < 232) {
      drawTVScreenPixel(draw_surface);
    }
    else if (ppu.draw.y < 240 && ppu.draw.x == 256) {
      ppu.scroll.x = ppu.scroll.temp_x;
    }
    else if (ppu.draw.x > 340) { // end of line, wrap
      //printf("%d scanline ended, clock cycles: %d %d\n", ppu.draw.y, cpu.clock_cycles, ppu.clock_cycles);
      ppu.draw.x = 0;
      ppu.draw.y += 1;
      continue;
    }
    else if (ppu.draw.y == 261) {
      if (ppu.draw.x == 0) ppu.status = 0;
      else if (ppu.draw.x >= 279 && ppu.draw.x <= 303) ppu.scroll.y = ppu.scroll.temp_y;
    }
    else if (ppu.draw.y > 261) { // end of screen, wrap
      assert(!ppu.draw.x);
      ppu.draw.y = 0;
      continue;
    }
    else if (ppu.draw.x == 339 && ppu.draw.y == 240) { // end of frame, vblank
      vblank(window, draw_surface, screen_surface);
    }
    else if (ppu.draw.x == 0 && ppu.draw.y == 241) { // first pixel after vblank
      ppu.status |= BIT7;
      ppu.nmi_occurred = true;
    }

    ppu.clock_cycles++;
    ppu.draw.x++;
  }
}
