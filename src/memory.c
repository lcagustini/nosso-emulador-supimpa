void writePPUByte(uint16_t addrs, uint8_t data){
  if (addrs >= 0x2000 && addrs < 0x2800) ppu.ram.data[addrs - 0x2000] = data;
  if (addrs >= 0x3000 && addrs < 0x3F00) writePPUByte(addrs - 0x1000, data);
  if (addrs >= 0x3F00) ppu.palette_ram[(addrs - 0x3F00) % 0x20] = data;
}

uint8_t readPPUByte(uint16_t addrs) {
  if (addrs < 0x2000) return cartridge.CHR[addrs];

  if (addrs < 0x2800) return ppu.ram.data[addrs - 0x2000];
  if (addrs >= 0x3000 && addrs < 0x3F00) return readPPUByte(addrs - 0x1000);

  if (addrs >= 0x3F00) {
    addrs = (addrs - 0x3F00) % 0x20;
    if (addrs == 0x10 || addrs == 0x14 || addrs == 0x18 || addrs == 0x1C) addrs -= 0x10;
    return ppu.palette_ram[addrs];
  }

  return 0;
}

uint8_t readCPUByte(uint16_t addrs) {
  if (addrs < 0x2000) return cpu.ram[addrs % 0x800];

  if (addrs == 0x2002) {
    uint8_t ppu_old = ppu.status;
    ppu.status &= ~BIT7;
    ppu.scroll.x = ppu.scroll.y = 0;
    ppu.ram.addrs = 0;

    return ppu_old;
  }
  if (addrs == 0x2004) return ppu.oam.data[ppu.oam.addrs];
  if (addrs == 0x2007) return readPPUByte(ppu.ram.addrs);

  if (addrs == 0x4016) return getNextInput1();
  if (addrs == 0x4017) return getNextInput2();
  if (addrs >= 0x4020) return cartridge.PRG[addrs-(0x10000-cartridge.PRG_size)];

  return 0;
}

void writeCPUByte(uint16_t addrs, uint8_t data) {
  if (addrs < 0x2000) cpu.ram[addrs % 0x800] = data;

  if (addrs == 0x2000) ppu.ctrl = data;
  if (addrs == 0x2001) ppu.mask = data;
  if (addrs == 0x2003) ppu.oam.addrs = data;
  if (addrs == 0x2004) {
    ppu.oam.data[ppu.oam.addrs] = data;
    ppu.oam.addrs++; // TODO: shouldn't happen during vblank or forced blank
  }
  if (addrs == 0x2005) {
    if (ppu.scroll.write_flag) ppu.scroll.y = data;
    else ppu.scroll.x = data;

    ppu.scroll.write_flag = !ppu.scroll.write_flag;
  }
  if (addrs == 0x2006) {
    if (ppu.ram.write_flag) ppu.ram.addrs = (ppu.ram.addrs & (~0xFF)) | data;
    else ppu.ram.addrs = (ppu.ram.addrs & (~0xFF00)) | (data << 8);

    ppu.scroll.y = ppu.scroll.x = 0;

    ppu.ram.write_flag = !ppu.ram.write_flag;
  }
  if (addrs == 0x2007) {
    writePPUByte(ppu.ram.addrs, data);
    ppu.ram.addrs += GET_VRAM_PPU_INCREMENT() ? 32 : 1;
  }
  if (addrs == 0x4014) oamDMA(data);
  if (addrs == 0x4016) input.poll_enable = data;
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
