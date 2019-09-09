uint8_t readCPUByte(uint16_t addrs) {
  return cpu.ram[addrs];
}

void writeCPUByte(uint16_t addrs, uint8_t data) {
  cpu.ram[addrs] = data;
}

uint8_t getInstructionByte() {
  uint8_t byte = cartridge.data[cpu.rb.pc];
  cpu.rb.pc++;
  return byte;
}

uint16_t getInstructionAddrs() {
  uint16_t addrs = cartridge.data[cpu.rb.pc];
  cpu.rb.pc++;
  addrs = (addrs << 8) | cartridge.data[cpu.rb.pc];
  cpu.rb.pc++;
  return addrs;
}
