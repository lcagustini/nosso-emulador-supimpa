void checkForInterrupts() {
  if (cpu.interrupt.irq) {
    uint16_t addrs = 0x0100 | cpu.rb.sp;
    writeCPUByte(addrs, (cpu.rb.pc >> 8) & 0xFF);
    cpu.rb.sp--;

    addrs = 0x0100 | cpu.rb.sp;
    writeCPUByte(addrs, cpu.rb.pc & 0xFF);
    cpu.rb.sp--;

    addrs = 0x0100 | cpu.rb.sp;
    writeCPUByte(addrs, (cpu.rb.p | BIT5) & (~BIT4));
    cpu.rb.sp--;

    cpu.rb.pc = readCPUByte(0xFFFE, false) | (readCPUByte(0xFFFF, false) << 8);

    cpu.interrupt.irq = false;

    cpu.clock_cycles += 7;
  }

  if (cpu.interrupt.nmi) {
    ppu.nmi_occurred = false;

    uint16_t addrs = 0x0100 | cpu.rb.sp;
    writeCPUByte(addrs, (cpu.rb.pc >> 8) & 0xFF);
    cpu.rb.sp--;

    addrs = 0x0100 | cpu.rb.sp;
    writeCPUByte(addrs, cpu.rb.pc & 0xFF);
    cpu.rb.sp--;

    addrs = 0x0100 | cpu.rb.sp;
    writeCPUByte(addrs, (cpu.rb.p | BIT5) & (~BIT4));
    cpu.rb.sp--;

    cpu.rb.pc = readCPUByte(0xFFFA, false) | (readCPUByte(0xFFFB, false) << 8);

    cpu.interrupt.nmi = false;

    cpu.clock_cycles += 7;
  }

  if (cpu.interrupt.brk) {
    uint16_t addrs = 0x0100 | cpu.rb.sp;
    writeCPUByte(addrs, (cpu.rb.pc >> 8) & 0xFF);
    cpu.rb.sp--;

    addrs = 0x0100 | cpu.rb.sp;
    writeCPUByte(addrs, cpu.rb.pc & 0xFF);
    cpu.rb.sp--;

    addrs = 0x0100 | cpu.rb.sp;
    writeCPUByte(addrs, cpu.rb.p | BIT5 | BIT4);
    cpu.rb.sp--;

    cpu.rb.pc = readCPUByte(0xFFFE, false) | (readCPUByte(0xFFFF, false) << 8);

    cpu.interrupt.brk = false;

    cpu.clock_cycles += 7;
  }
}
