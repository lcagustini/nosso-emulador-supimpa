uint8_t readCPUByte(uint16_t addrs) {
  if (addrs < 0x800) return cpu.ram[addrs];
  if (addrs >= 0x8000) return cartridge.data[addrs-0x7FF0];
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

uint8_t zeroPageByte(uint8_t addrs) {
  return readCPUByte(addrs);
}

uint8_t zeroPageByteWithX(uint8_t addrs) {
  return readCPUByte(addrs + cpu.rb.x);
}

uint8_t absoluteByte(uint16_t addrs) {
  return readCPUByte(addrs);
}

uint8_t absoluteByteWithX(uint16_t addrs) {
  return readCPUByte(addrs + cpu.rb.x);
}

uint8_t absoluteByteWithY(uint16_t addrs) {
  return readCPUByte(addrs + cpu.rb.y);
}

uint8_t indirectByteWithX(uint8_t addrs) {
  uint8_t lobyte = readCPUByte(addrs + cpu.rb.x);
  uint8_t hibyte = readCPUByte(addrs + cpu.rb.x + 1);
  uint16_t indirectAddrs = (hibyte << 8) | lobyte;

  return readCPUByte(indirectAddrs);
}

uint8_t indirectByteWithY(uint8_t addrs) {
  uint8_t lobyte = readCPUByte(addrs);
  uint8_t hibyte = readCPUByte(addrs + 1);
  uint16_t indirectAddrs = ((hibyte << 8) | lobyte) + cpu.rb.y;

  return readCPUByte(indirectAddrs);
}
