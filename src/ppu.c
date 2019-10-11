// PPUCTRL
#define GET_BASE_NAMETABLE_ID() (ppu.ctrl & 0b11)
#define SET_BASE_NAMETABLE_ID(a) (ppu.ctrl = (ppu.ctrl & (~0b11)) | (a & 0b11))
#define NAMETABLE_ID_TO_ADDRS(a) ( \
      a == 0 ? 0x2000 : ( \
      a == 1 ? 0x2400 : ( \
      a == 2 ? 0x2800 : 0x2C00 )) \
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

// PPUSTATUS
// TODO: Implement the 5 least significant bits
#define GET_SPRITE_OVERFLOW() (ppu.status & BIT5)
#define GET_SPRITE_0_HIT() (ppu.status & BIT6)
#define GET_VBLANK_START() (ppu.status & BIT7)

void oamDMA(uint8_t hibyte) {
  uint16_t addrs = (hibyte << 8);

  for (int i = oam_addrs; i < 0x100; i++) {
    ppu.oam[i] = cpu.ram[addrs];
  }

  if (cpu.clock_cycles % 2) cpu.clock_cycles += 1;
  cpu.clock_cycles += 513;
}
