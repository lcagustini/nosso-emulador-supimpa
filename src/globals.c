struct {
  struct {
    uint8_t a;
    uint8_t x;
    uint8_t y;

    uint8_t p;
    uint8_t sp;

    uint16_t pc;
  } rb;

  struct {
    bool irq;
    bool nmi;
    bool brk;
  } interrupt;

  uint8_t ram[CPU_RAM];
} cpu;

struct {
  uint8_t *PRG; // PRG-ROM banks
  uint32_t PRG_size;

  uint8_t *CHR; // CHR-ROM banks
  uint32_t CHR_size;

  uint8_t control1;
  uint8_t control2;

  uint8_t mapper;
  uint8_t mirror;
  uint8_t battery;
} cartridge;
