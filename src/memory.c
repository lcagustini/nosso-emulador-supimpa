uint8_t readCPUByte(uint16_t addrs) {
  if (addrs < 0x2000) return cpu.ram[addrs % 0x800];

  if (addrs >= 0x4020) return cartridge.PRG[addrs-(0x10000-cartridge.PRG_size)];

  if (addrs == 0x2002) return ppu.status;
  if (addrs == 0x2004) return ppu.oam[ppu.oam_addrs];
  if (addrs == 0x2007) return ppu.ram[ppu.ram_addrs - 0x2000];

  return 0;
}

void writeCPUByte(uint16_t addrs, uint8_t data) {
  if (addrs < 0x2000) cpu.ram[addrs % 0x800] = data;

  if (addrs == 0x2000) ppu.ctrl = data;
  if (addrs == 0x2001) ppu.mask = data;
  if (addrs == 0x2003) ppu.oam_addrs = data;
  if (addrs == 0x2004) {
    ppu.oam[ppu.oam_addrs] = data;
    ppu.oam_addrs++; // TODO: shouldn't happen during vblank or forced blank
  }
  if (addrs == 0x2005) {
    if (ppu.scroll_write_flag) ppu.scroll_y = data;
    else ppu.scroll_x = data;

    ppu.scroll_write_flag = !ppu.scroll_write_flag;
  }
  if (addrs == 0x2006) {
    if (ppu.ram_write_flag) ppu.ram_addrs = (ppu.ram_addrs & (~0xFF)) | data;
    else ppu.ram_addrs = (ppu.ram_addrs & (~0xFF00)) | (data << 8);

    ppu.scroll_y = ppu.scroll_x = 0;

    ppu.ram_write_flag = !ppu.ram_write_flag;
  }
  if (addrs == 0x2007) {
    ppu.ram[ppu.ram_addrs - 0x2000] = data;
    ppu.ram_addrs += GET_VRAM_PPU_INCREMENT() ? 32 : 1;
  }
  if (addrs == 0x4014) oamDMA(data);
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
