void writePPUByte(uint16_t addrs, uint8_t data) {
  if (addrs >= 0x2000 && addrs < 0x3000) {
    addrs -= 0x2000;
    if (!cartridge.mirror) {
      if ((addrs >= 0x400 && addrs < 0x800) || addrs >= 0xC00) addrs -= 0x400;
    }
    else if ((addrs >= 0x800 && addrs < 0xC00) || addrs >= 0xC00) addrs -= 0x800;
    assert(addrs < 0xC00);
    ppu.ram.data[addrs] = data;
  }
  else if (addrs >= 0x3000 && addrs < 0x3F00) writePPUByte(addrs - 0x1000, data);
  else if (addrs >= 0x3F00) {
    addrs = (addrs - 0x3F00) % 0x20;
    if (addrs == 0x10 || addrs == 0x14 || addrs == 0x18 || addrs == 0x1C) addrs -= 0x10;
    ppu.palette_ram[addrs] = data;
  }
}

uint8_t readPPUByte(uint16_t addrs, bool internal_read) {
  if (addrs < 0x3F00) {
    uint8_t data = ppu.ram.buffer;
    uint8_t temp;
mirror:
    if (addrs < 0x2000) temp = cartridge.CHR[addrs];
    else if (addrs < 0x3000) {
      addrs -= 0x2000;
      if (!cartridge.mirror) {
        if ((addrs >= 0x400 && addrs < 0x800) || addrs >= 0xC00) addrs -= 0x400;
      }
      else if ((addrs >= 0x800 && addrs < 0xC00) || addrs >= 0xC00) addrs -= 0x800;
      assert(addrs < 0xC00);
      temp = ppu.ram.data[addrs];
    }
    else {
      addrs -= 0x1000;
      goto mirror;
    }

    if (!internal_read) {
      ppu.ram.buffer = temp;
      return data;
    }
    else return temp;
  }
  else {
    addrs = (addrs - 0x3F00) % 0x20;
    if (addrs == 0x10 || addrs == 0x14 || addrs == 0x18 || addrs == 0x1C) addrs -= 0x10;
    return ppu.palette_ram[addrs];
  }
}

uint8_t readCPUByte(uint16_t addrs, bool internal_read) {
  if (addrs < 0x2000) return cpu.ram[addrs % 0x800];

  if (addrs == 0x2002) {
    uint8_t ppu_old = ppu.status;

    if (!internal_read) {
      ppu.nmi_occurred = false;
      ppu.status &= ~BIT7;
      ppu.write_flag = 0;
    }

    return ppu_old;
  }
  if (addrs == 0x2004) return internal_read ? ppu.oam.data[ppu.oam.addrs] : ppu.oam.data[ppu.oam.addrs++];
  if (addrs == 0x2007) {
    uint8_t data = readPPUByte(ppu.ram.addrs, internal_read);
    if (!internal_read) ppu.ram.addrs += GET_VRAM_PPU_INCREMENT() ? 32 : 1;
    return data;
  }
  if (addrs >= 0x2008 && addrs < 0x4000) readCPUByte(0x2000 + (addrs % 8), internal_read);

  if (addrs == 0x4016) return internal_read ? 0 : getNextInput1();
  if (addrs == 0x4017) return internal_read ? 0 : getNextInput2();
  if (addrs >= 0x4020) return cartridge.PRG[addrs % cartridge.PRG_size];

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
    if (ppu.write_flag) ppu.scroll.temp_y = data;
    else {
      ppu.scroll.temp_x = data;
      //ppu.scroll.x = (ppu.scroll.x & (~0b111)) | (data & 0b111);
    }

    ppu.write_flag = !ppu.write_flag;
  }
  if (addrs == 0x2006) {
    if (ppu.write_flag) {
      ppu.ram.addrs = (ppu.ram.addrs & (~0xFF)) | data;
      ppu.scroll.y = data;
    }
    else {
      ppu.ram.addrs = (ppu.ram.addrs & (~0xFF00)) | (data << 8);
      ppu.scroll.x = (ppu.scroll.x & 0b111) | (data & (~0b111));
    }

    ppu.write_flag = !ppu.write_flag;
  }
  if (addrs == 0x2007) {
    writePPUByte(ppu.ram.addrs, data);
    ppu.ram.addrs += GET_VRAM_PPU_INCREMENT() ? 32 : 1;
  }
  if (addrs >= 0x2008 && addrs < 0x4000) writeCPUByte(0x2000 + (addrs % 8), data);
  if (addrs == 0x4014) oamDMA(data);
  if (addrs == 0x4016) {
    if (data) pollInput();
  }
}

uint8_t getInstructionByte() {
  uint8_t byte = readCPUByte(cpu.rb.pc, true);
  cpu.rb.pc++;
  return byte;
}

uint16_t getInstructionAddrs() {
  uint16_t addrs = readCPUByte(cpu.rb.pc, true);
  cpu.rb.pc++;
  addrs = addrs | (readCPUByte(cpu.rb.pc, true) << 8);
  cpu.rb.pc++;
  return addrs;
}
