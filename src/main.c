#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define bool uint8_t
#define true 1
#define false 0

#define COLOR_BLUE "\033[0;34m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_YELLOW "\033[01;33m"
#define COLOR_RESET "\033[0m"

#define CPU_RAM 0x800
#define PPU_RAM 0x800
#define OAM_RAM 256

#define WINDOW_ZOOM 3
#define NES_WIDTH 256
#define NES_HEIGHT 224

#define BIT0 0b1
#define BIT1 0b10
#define BIT2 0b100
#define BIT3 0b1000
#define BIT4 0b10000
#define BIT5 0b100000
#define BIT6 0b1000000
#define BIT7 0b10000000

//#define OPCODE_PRINT
#define DEBUG_PRINT

void print(uint8_t a, uint8_t x, uint8_t y, uint16_t sp, uint16_t pc, uint8_t p);
void printls(uint8_t a, uint8_t x, uint8_t y, uint16_t sp, uint16_t pc, uint8_t p, uint16_t addr, uint8_t data);

#include "globals.c"
#include "memory.c"
#include "cpu.c"
#include "interrupt.c"
#include "ppu.c"

#ifdef OPCODE_PRINT
static char optable[256][256] = {
  /*        |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  A  |  B  |  C  |  D  |  E  |  F  |      */
  /* 0 */      "brk",  "ora",  "nop",  "slo",  "nop",  "ora",  "asl",  "slo",  "php",  "ora",  "asl",  "nop",  "nop",  "ora",  "asl",  "slo", /* 0 */
  /* 1 */      "bpl",  "ora",  "nop",  "slo",  "nop",  "ora",  "asl",  "slo",  "clc",  "ora",  "nop",  "slo",  "nop",  "ora",  "asl",  "slo", /* 1 */
  /* 2 */      "jsr",  "and",  "nop",  "rla",  "bit",  "and",  "rol",  "rla",  "plp",  "and",  "rol",  "nop",  "bit",  "and",  "rol",  "rla", /* 2 */
  /* 3 */      "bmi",  "and",  "nop",  "rla",  "nop",  "and",  "rol",  "rla",  "sec",  "and",  "nop",  "rla",  "nop",  "and",  "rol",  "rla", /* 3 */
  /* 4 */      "rti",  "eor",  "nop",  "sre",  "nop",  "eor",  "lsr",  "sre",  "pha",  "eor",  "lsr",  "nop",  "jmp",  "eor",  "lsr",  "sre", /* 4 */
  /* 5 */      "bvc",  "eor",  "nop",  "sre",  "nop",  "eor",  "lsr",  "sre",  "cli",  "eor",  "nop",  "sre",  "nop",  "eor",  "lsr",  "sre", /* 5 */
  /* 6 */      "rts",  "adc",  "nop",  "rra",  "nop",  "adc",  "ror",  "rra",  "pla",  "adc",  "ror",  "nop",  "jmp",  "adc",  "ror",  "rra", /* 6 */
  /* 7 */      "bvs",  "adc",  "nop",  "rra",  "nop",  "adc",  "ror",  "rra",  "sei",  "adc",  "nop",  "rra",  "nop",  "adc",  "ror",  "rra", /* 7 */
  /* 8 */      "nop",  "sta",  "nop",  "sax",  "sty",  "sta",  "stx",  "sax",  "dey",  "nop",  "txa",  "nop",  "sty",  "sta",  "stx",  "sax", /* 8 */
  /* 9 */      "bcc",  "sta",  "nop",  "nop",  "sty",  "sta",  "stx",  "sax",  "tya",  "sta",  "txs",  "nop",  "nop",  "sta",  "nop",  "nop", /* 9 */
  /* A */      "ldy",  "lda",  "ldx",  "lax",  "ldy",  "lda",  "ldx",  "lax",  "tay",  "lda",  "tax",  "nop",  "ldy",  "lda",  "ldx",  "lax", /* A */
  /* B */      "bcs",  "lda",  "nop",  "lax",  "ldy",  "lda",  "ldx",  "lax",  "clv",  "lda",  "tsx",  "lax",  "ldy",  "lda",  "ldx",  "lax", /* B */
  /* C */      "cpy",  "cmp",  "nop",  "dcp",  "cpy",  "cmp",  "dec",  "dcp",  "iny",  "cmp",  "dex",  "nop",  "cpy",  "cmp",  "dec",  "dcp", /* C */
  /* D */      "bne",  "cmp",  "nop",  "dcp",  "nop",  "cmp",  "dec",  "dcp",  "cld",  "cmp",  "nop",  "dcp",  "nop",  "cmp",  "dec",  "dcp", /* D */
  /* E */      "cpx",  "sbc",  "nop",  "isb",  "cpx",  "sbc",  "inc",  "isb",  "inx",  "sbc",  "nop",  "sbc",  "cpx",  "sbc",  "inc",  "isb", /* E */
  /* F */      "beq",  "sbc",  "nop",  "isb",  "nop",  "sbc",  "inc",  "isb",  "sed",  "sbc",  "nop",  "isb",  "nop",  "sbc",  "inc",  "isb  "/* F */
};
#endif

void loadNESFile(char *filepath) {
  uint8_t null_buffer[512];

  FILE *rom_file = fopen(filepath, "rb");
  if (!rom_file) {
    fprintf(stderr, COLOR_RED "No file found!\n");
    exit(1);
  }

  fread(null_buffer, 1, 4, rom_file);

  fread(&cartridge.PRG_size, 1, 1, rom_file);
  cartridge.PRG_size *= 0x4000;

  fread(&cartridge.CHR_size, 1, 1, rom_file);
  cartridge.CHR_size *= 0x2000;

  fread(&cartridge.control1, 1, 1, rom_file);
  fread(&cartridge.control2, 1, 1, rom_file);

  fread(null_buffer, 1, 8, rom_file);

  cartridge.mapper = (cartridge.control1 >> 4) | ((cartridge.control2 >> 4) << 4);
  cartridge.mirror = (cartridge.control1 & 1) | (((cartridge.control1 >> 3) & 1) << 1);
  cartridge.battery = (cartridge.control1 >> 1) & 1;

  if (cartridge.control1 & 4) {
    fread(null_buffer, 1, 512, rom_file);
  }

  cartridge.PRG = malloc(cartridge.PRG_size);
  fread(cartridge.PRG, 1, cartridge.PRG_size, rom_file);

  cartridge.CHR = malloc(cartridge.CHR_size);
  fread(cartridge.CHR, 1, cartridge.CHR_size, rom_file);

  cpu.clock_cycles = 0;
}

int main(int argc, char* argv[]) {
  SDL_Window *window = NULL;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    exit(1);
  }

  window = SDL_CreateWindow("nosso-emulador-supimpa", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      WINDOW_ZOOM*NES_WIDTH, WINDOW_ZOOM*NES_HEIGHT, SDL_WINDOW_SHOWN /*| SDL_WINDOW_FULLSCREEN*/);

  if (!window) {
    fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_Surface *screen_surface = SDL_GetWindowSurface(window);
  SDL_Surface *draw_surface = SDL_CreateRGBSurface(0, NES_WIDTH, NES_HEIGHT,
      screen_surface->format->BitsPerPixel,
      screen_surface->format->Rmask, screen_surface->format->Gmask,
      screen_surface->format->Bmask, screen_surface->format->Amask);
  SDL_Event e;

  if (argc <= 1) {
    fprintf(stderr, COLOR_RED "Rom file needed!\n");
    exit(1);
  }

  loadNESFile(argv[1]);

  cpu.rb.p = 0x34;
  cpu.rb.sp = 0xfd;

reset:
  cpu.rb.pc = readCPUByte(0xFFFC) | (readCPUByte(0xFFFD) << 8);
  while (true) {
    while(SDL_PollEvent(&e)){
      if(e.type == SDL_QUIT)
        exit(0);
    }

    uint8_t opcode = getInstructionByte();
#ifdef OPCODE_PRINT
    printf("%s ", optable[opcode]);
#endif
    doInstruction(opcode);
    checkForInterrupts();
  }

  free(cartridge.PRG);
  free(cartridge.CHR);

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
