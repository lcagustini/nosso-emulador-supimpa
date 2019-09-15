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

#define BIT0 0b1
#define BIT1 0b10
#define BIT2 0b100
#define BIT3 0b1000
#define BIT4 0b10000
#define BIT5 0b100000
#define BIT6 0b1000000
#define BIT7 0b10000000

#define DEBUG_PRINT

void print(uint8_t a, uint8_t x, uint8_t y, uint16_t sp, uint16_t pc, uint8_t p);
void printls(uint8_t a, uint8_t x, uint8_t y, uint16_t sp, uint16_t pc, uint8_t p, uint16_t addr, uint8_t data);

#include "globals.c"
#include "memory.c"
#include "cpu.c"
#include "interrupt.c"

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
}

int main(int argc, char* argv[]) {
  if (argc <= 1) {
    fprintf(stderr, COLOR_RED "Rom file needed!\n");
    exit(1);
  }

  loadNESFile(argv[1]);

reset:
  cpu.rb.pc = readCPUByte(0xFFFC) | (readCPUByte(0xFFFD) << 8);
  while (true) {
    uint8_t opcode = getInstructionByte();
    doInstruction(opcode);
    checkForInterrupts();
  }

  free(cartridge.PRG);
  free(cartridge.CHR);

  return 0;
}
