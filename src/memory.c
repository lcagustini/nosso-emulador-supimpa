uint8_t readCPUByte(uint16_t addrs) {
  if (addrs < 0x800) return cpu.ram[addrs];
  if (addrs >= 0x4020) return cartridge.PRG[addrs-(0x10000-cartridge.PRG_size)];
  return 0;
}

void writeCPUByte(uint16_t addrs, uint8_t data) {
  if (addrs < 0x800) cpu.ram[addrs] = data;
}

uint8_t getInstructionByte() {
  uint8_t byte = readCPUByte(cpu.rb.pc);
  cpu.rb.pc++;
  return byte;
}

uint16_t getInstructionAddrs() {
  uint16_t addrs = readCPUByte(cpu.rb.pc);
  cpu.rb.pc++;
  addrs = addrs | (readCPUByte(cpu.rb.pc) << 8);
  cpu.rb.pc++;
  return addrs;
}
