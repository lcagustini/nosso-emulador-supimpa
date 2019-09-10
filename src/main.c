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

void print(uint8_t a, uint8_t x, uint8_t y, uint16_t sp, uint16_t pc, uint8_t p);
void printls(uint8_t a, uint8_t x, uint8_t y, uint16_t sp, uint16_t pc, uint8_t p, uint16_t addr, uint8_t data);

struct {
  struct {
    uint8_t a;
    uint8_t x;
    uint8_t y;

    uint8_t p;
    uint8_t sp;

    uint16_t pc;
  } rb;

  uint8_t ram[CPU_RAM];
} cpu;

struct {
  uint8_t data[CARTRIDGE_SIZE];
  uint64_t data_size;
} cartridge;

#include "memory.c"
#include "cpu.c"

int main(int argc, char* argv[]) {
  if (argc <= 1) {
    fprintf(stderr, COLOR_RED "Rom file needed!\n");
    return 1;
  }

  FILE *rom_file = fopen(argv[1], "rb");

  if (!rom_file) {
    fprintf(stderr, COLOR_RED "No file found!\n");
    return 1;
  }

  cartridge.data_size = fread(cartridge.data, 1, sizeof(cartridge.data), rom_file);
  fclose(rom_file);

  cpu.rb.pc = readCPUByte(0xFFFC) | readCPUByte(0xFFFD) << 8;

  while (true) {
    uint8_t opcode = getInstructionByte();

    doInstruction(opcode);
  }

  print(0xFF, 0xEE, 0xDD, 0xCCCC, 0xBBBB, 0xAA);
  printls(0xFF, 0xEE, 0xDD, 0xCCCC, 0xBBBB, 0xAA, 0xFFFF, 0x99);

  return 0;
}
