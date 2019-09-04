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

int main(int argc, char* argv[]) {
  if (argc <= 1) {
    fprintf(stderr, "Rom file needed!\n");
    return 1;
  }

  FILE *rom = fopen(argv[1], "rb");

  return 0;
}
