uint32_t grayscale(uint32_t color) {
  uint8_t r = color & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = (color >> 16) & 0xFF;

  uint8_t y = (2126*r+7152*g+722*b)/10000;

  return y | (y << 8) | (y << 16);
}

uint32_t emphasize(uint32_t color) {
  uint8_t emph_b = (ppu.mask & BIT5) >> 5;
  uint8_t emph_g = (ppu.mask & BIT6) >> 6;
  uint8_t emph_r = (ppu.mask & BIT7) >> 7;

  uint8_t r = color & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = (color >> 16) & 0xFF;

  if (emph_r + emph_g + emph_b == 2) {
    r = (r << emph_r) >> (emph_g | emph_b);
    g = (g << emph_g) >> (emph_b | emph_r);
    b = (b << emph_b) >> (emph_g | emph_r);
  }
  else {
    r = r >> (emph_g | emph_b);
    g = g >> (emph_b | emph_r);
    b = b >> (emph_g | emph_r);
  }

  return r | (g << 8) | (b << 16);
}
