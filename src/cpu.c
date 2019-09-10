void doInstruction(uint8_t opcode) {
  switch (opcode) {
    default:
      fprintf(stderr, COLOR_RED "Unimplemented OP Code: 0x%X\n", opcode);
      exit(1);
  }
}
