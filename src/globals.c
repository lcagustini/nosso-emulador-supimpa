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
  uint8_t data[CARTRIDGE_SIZE];
  uint64_t data_size;
} cartridge;
