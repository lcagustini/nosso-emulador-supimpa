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

#define CPU_RAM 8000 //should look up the correct size

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

int main(int argc, char* argv[]) {
  if (argc <= 1) {
    fprintf(stderr, COLOR_RED "Rom file needed!\n");
    return 1;
  }

  FILE *rom_file = fopen(argv[1], "rb");

  return 0;
}
