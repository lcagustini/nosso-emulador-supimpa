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

#define CPU_RAM 0x800 //should look up the correct size
#define CARTRIDGE_SIZE 30 * (1 << 20) // 30MB

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

int main(int argc, char* argv[]) {
  if (argc <= 1) {
    fprintf(stderr, COLOR_RED "Rom file needed!\n");
    exit(1);
  }

  FILE *rom_file = fopen(argv[1], "rb");
  if (!rom_file) {
    fprintf(stderr, COLOR_RED "No file found!\n");
    exit(1);
  }
  cartridge.data_size = fread(cartridge.data, 1, sizeof(cartridge.data), rom_file);
  fclose(rom_file);

reset:
  cpu.rb.pc = readCPUByte(0xFFFC) | (readCPUByte(0xFFFD) << 8);
  while (true) {
    uint8_t opcode = getInstructionByte();
    doInstruction(opcode);
    checkForInterrupts();
  }

  return 0;
}
