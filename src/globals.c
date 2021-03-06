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

  uint64_t clock_cycles;
} cpu;

struct {
  uint8_t *PRG; // PRG-ROM banks
  uint32_t PRG_size;

  uint8_t *CHR; // CHR-ROM banks
  uint32_t CHR_size;

  uint8_t control1;
  uint8_t control2;

  bool chr_ram;

  uint8_t mapper;
  uint8_t mirror;
  uint8_t battery;
} cartridge;

struct {
  uint8_t ctrl;
  uint8_t mask;
  uint8_t status;

  bool write_flag;

  bool nmi_occurred;
  uint8_t scanline_sprite_count;

  struct {
    uint16_t temp_x;
    uint16_t temp_y;

    uint16_t x;
    uint16_t y;
  } scroll;

  struct {
    uint16_t addrs;
    uint8_t temp_addrs;

    uint8_t data[PPU_RAM];
    uint8_t buffer;
  } ram;

  struct {
    uint8_t addrs;
    uint8_t data[OAM_RAM];
  } oam;

  struct {
    uint16_t x;
    uint16_t y;
  } draw;

  uint8_t palette_ram[32];

  uint64_t clock_cycles;
} ppu;

struct {
  uint8_t shift_register1;
  uint8_t shift_register2;
} input;

typedef enum {
  P_NO_SPRITE,
  P_OVER_BG,
  P_UNDER_BG,
} priority_t;

// NOTE: currently we sleep too much sometimes because of system related stuff
//       not sure what to do about it
struct timeval last_frame_time;
