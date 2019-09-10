void doInstruction(uint8_t opcode) {
  printf(COLOR_BLUE);
  switch (opcode) {
    case 0x78: // sei
      cpu.rb.p |= 0b100;
      break;
    case 0x8E: // srx abs
      writeCPUByte(getInstructionAddrs(), cpu.rb.x);
      break;
    case 0x9A: // txs
      cpu.rb.sp = cpu.rb.x;
      break;
    case 0xA2: // ldx #imm
      cpu.rb.x = getInstructionByte();
      break;
    case 0xD8: // cld
      cpu.rb.p &= ~0b1000;
      break;
    case 0xE8: // inx
      cpu.rb.x++;
      break;
    default:
      fprintf(stderr, COLOR_RED "Unimplemented OP Code: 0x%X\n", opcode);
      exit(1);
  }
}
