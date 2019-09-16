#define SET_C() (cpu.rb.p |= BIT0)
#define CLEAR_C() (cpu.rb.p &= ~BIT0)
#define GET_C() (cpu.rb.p | BIT0)

#define SET_Z() (cpu.rb.p |= BIT1)
#define CLEAR_Z() (cpu.rb.p &= ~BIT1)
#define GET_Z() ((cpu.rb.p | BIT1) >> 1)

#define SET_I() (cpu.rb.p |= BIT2)
#define CLEAR_I() (cpu.rb.p &= ~BIT2)
#define GET_I() ((cpu.rb.p | BIT2) >> 2)

#define SET_D() (cpu.rb.p |= BIT3)
#define CLEAR_D() (cpu.rb.p &= ~BIT3)
#define GET_D() ((cpu.rb.p | BIT3) >> 3)

#define SET_V() (cpu.rb.p |= BIT6)
#define CLEAR_V() (cpu.rb.p &= ~BIT6)
#define GET_V() ((cpu.rb.p | BIT6) >> 6)

#define SET_N() (cpu.rb.p |= BIT7)
#define CLEAR_N() (cpu.rb.p &= ~BIT7)
#define GET_N() ((cpu.rb.p | BIT7) >> 7)

#define UPDATE_Z_FLAG(a) { \
  if (a) CLEAR_Z();        \
  else SET_Z();            \
}

#define UPDATE_N_FLAG(a) { \
  if (a < 128) CLEAR_N();  \
  else SET_N();            \
}

#define ADD_SIGNED_TO_UNSIGNED(a, b) { \
  if (a < 128) b += a;                 \
  else b -= (~a + 1);                  \
}

void doInstruction(uint8_t opcode) {
  switch (opcode) {
    case 0x00: // brk
      {
        cpu.interrupt.brk = true;

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x01: // ora x, ind
      {
        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte + cpu.rb.x);
        uint16_t addrs = readCPUByte(byte + cpu.rb.x + 1);
        addrs <<= 8;
        addrs |= low;
        cpu.rb.a |= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x05: // ora zpg
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        cpu.rb.a |= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x06: // asl zpg
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        uint8_t tmp = readCPUByte(addrs);
        if (tmp & BIT7) SET_C();
        else CLEAR_C();
        tmp <<= 1;

        UPDATE_N_FLAG(tmp);
        UPDATE_Z_FLAG(tmp);

        writeCPUByte(addrs, tmp);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x08: //php impl
      {
        uint16_t addrs = 0x0100 | cpu.rb.sp;
        writeCPUByte(addrs, cpu.rb.p | BIT5 | BIT4);
        cpu.rb.sp--;

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x09: // ora #
      {

        uint8_t imm = getInstructionByte();
        cpu.rb.a |= imm;

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x0A: // asl a
      {
        if (cpu.rb.a & BIT7) SET_C();
        else CLEAR_C();
        cpu.rb.a <<= 1;

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x0D: // ora abs
      {
        uint16_t addrs = getInstructionAddrs();
        cpu.rb.a |= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x0E: // asl abs
      {
        uint16_t addrs = getInstructionAddrs();
        uint8_t mem = readCPUByte(addrs);
        if (mem & BIT7) SET_C();
        else CLEAR_C();
        mem <<= 1;

        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);

        writeCPUByte(addrs, mem);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x10: // bpl dest
      {
        uint8_t dest = getInstructionByte();
        if (!GET_N()) ADD_SIGNED_TO_UNSIGNED(dest, cpu.rb.pc);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x11: // ora ind, y
      {
        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte);
        uint16_t addrs = readCPUByte(byte + 1);
        addrs <<= 8;
        addrs |= low;
        addrs += cpu.rb.y + GET_C();
        cpu.rb.a |= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x15: // ora zpg, x
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        addrs += cpu.rb.x;
        cpu.rb.a |= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x16: // asl zpg, x
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        addrs += cpu.rb.x;
        uint8_t tmp = readCPUByte(addrs);
        if (tmp & BIT7) SET_C();
        else CLEAR_C();
        tmp <<= 1;

        UPDATE_N_FLAG(tmp);
        UPDATE_Z_FLAG(tmp);

        writeCPUByte(addrs, tmp);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x18: //clc impl
      {
        CLEAR_C();

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x19: // ora abs, y
      {

        uint16_t addrs = getInstructionAddrs() + cpu.rb.y;
        cpu.rb.a |= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x1D: // ora abs, x
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        cpu.rb.a |= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x1E: // asl abs, x
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        uint8_t mem = readCPUByte(addrs);
        if (mem & BIT7) SET_C();
        else CLEAR_C();
        mem <<= 1;

        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);

        writeCPUByte(addrs, mem);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x21: // and x, ind
      {
        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte + cpu.rb.x);
        uint16_t addrs = readCPUByte(byte + cpu.rb.x + 1);
        addrs <<= 8;
        addrs |= low;
        cpu.rb.a &= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x25: // and zpg
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        cpu.rb.a &= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x26: // rol zpg
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        uint8_t tmp = readCPUByte(addrs);

        uint8_t carry = tmp & BIT7;
        tmp <<= 1;
        tmp |= GET_C();

        if (carry) SET_C();
        else CLEAR_C();

        UPDATE_N_FLAG(tmp);
        UPDATE_Z_FLAG(tmp);

        writeCPUByte(addrs, tmp);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x28: //plp impl
      {
        cpu.rb.sp++;
        uint16_t addrs = 0x0100 | cpu.rb.sp;
        cpu.rb.p = readCPUByte(addrs) & (~BIT5) & (~BIT4);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x29: // and #
      {
        uint8_t imm = getInstructionByte();
        cpu.rb.a &= imm;

        UPDATE_Z_FLAG(cpu.rb.a);
        UPDATE_N_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x2A: // rol a
      {
        uint8_t carry = cpu.rb.a & BIT7;
        cpu.rb.a <<= 1;
        cpu.rb.a |= GET_C();

        if (carry) SET_C();
        else CLEAR_C();
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x2C: // bit abs
      {
        uint16_t addrs = getInstructionAddrs();
        uint8_t mem = readCPUByte(addrs);
        cpu.rb.p = (cpu.rb.p & ~(BIT7|BIT6)) | (mem & (BIT7|BIT6));

        UPDATE_Z_FLAG(cpu.rb.a & mem);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x2D: // and abs
      {
        uint16_t addrs = getInstructionAddrs();
        cpu.rb.a &= readCPUByte(addrs);

        UPDATE_Z_FLAG(cpu.rb.a);
        UPDATE_N_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x2E: // rol abs
      {
        uint16_t addrs = getInstructionAddrs();
        uint8_t mem = readCPUByte(addrs);
        uint8_t carry = mem & BIT7;
        mem <<= 1;
        mem |= GET_C();

        if (carry) SET_C();
        else CLEAR_C();
        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);

        writeCPUByte(addrs, mem);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x31: // and ind, y
      {
        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte);
        uint16_t addrs = readCPUByte(byte + 1);
        addrs <<= 8;
        addrs |= low;
        addrs += cpu.rb.y + GET_C();
        cpu.rb.a &= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x35: // and zpg, x
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        addrs += cpu.rb.x;
        cpu.rb.a &= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x36: // rol zpg, x
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        addrs += cpu.rb.x;
        uint8_t tmp = readCPUByte(addrs);

        uint8_t carry = tmp & BIT7;
        tmp <<= 1;
        tmp |= GET_C();

        if (carry) SET_C();
        else CLEAR_C();

        UPDATE_N_FLAG(tmp);
        UPDATE_Z_FLAG(tmp);

        writeCPUByte(addrs, tmp);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x38: //sec
      {
        SET_C();

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x39: // and abs, y
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.y;
        cpu.rb.a &= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x3D: // and abs, x
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        cpu.rb.a &= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x3E: // rol abs, x
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        uint8_t mem = readCPUByte(addrs);
        uint8_t carry = mem & BIT7;
        mem <<= 1;
        mem |= GET_C();

        if (carry) SET_C();
        else CLEAR_C();
        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);

        writeCPUByte(addrs, mem);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x40: // rti
      {
        cpu.rb.sp++;
        uint16_t addrs = 0x0100 | cpu.rb.sp;
        cpu.rb.p = readCPUByte(addrs) & (~BIT4) & (~BIT5);

        cpu.rb.sp++;
        addrs = 0x0100 | cpu.rb.sp;
        cpu.rb.pc = readCPUByte(addrs);

        cpu.rb.sp++;
        addrs = 0x0100 | cpu.rb.sp;
        cpu.rb.pc |= readCPUByte(addrs) << 8;

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x41: // eor x, ind
      {
        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte + cpu.rb.x);
        uint16_t addrs = readCPUByte(byte + cpu.rb.x + 1);
        addrs <<= 8;
        addrs |= low;
        cpu.rb.a ^= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x45: // eor zpg
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        cpu.rb.a ^= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x46: // lsr zpg
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        uint8_t tmp = readCPUByte(addrs);

        if (tmp & BIT0) SET_C();
        else CLEAR_C();
        tmp >>= 1;

        CLEAR_N();
        UPDATE_Z_FLAG(cpu.rb.a);

        writeCPUByte(addrs, tmp);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x48: //pha impl
      {
        uint16_t addrs = 0x0100 | cpu.rb.sp;
        writeCPUByte(addrs, cpu.rb.a);
        cpu.rb.sp--;

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x49: // eor #
      {
        uint8_t imm = getInstructionByte();
        cpu.rb.a ^= imm;

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x4C: // jmp abs
      {
        cpu.rb.pc = getInstructionAddrs();

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x4A: // lsr a
      {
        if (cpu.rb.a & BIT0) SET_C();
        else CLEAR_C();
        cpu.rb.a >>= 1;

        CLEAR_N();
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x4D: // eor abs
      {
        uint16_t addrs = getInstructionAddrs();
        cpu.rb.a ^= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x4E: // lsr abs
      {
        uint16_t addrs = getInstructionAddrs();
        uint8_t mem = readCPUByte(addrs);
        if (mem & BIT0) SET_C();
        else CLEAR_C();
        mem >>= 1;
        CLEAR_N();
        UPDATE_Z_FLAG(mem);
        writeCPUByte(addrs, mem);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x51: // eor ind, y
      {
        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte);
        uint16_t addrs = readCPUByte(byte + 1);
        addrs <<= 8;
        addrs |= low;
        addrs += cpu.rb.y + GET_C();
        cpu.rb.a ^= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x55: // eor zpg, x
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        addrs += cpu.rb.x;
        cpu.rb.a ^= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x56: // lsr zpg, x
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        addrs += cpu.rb.x;
        uint8_t tmp = readCPUByte(addrs);

        if (tmp & BIT0) SET_C();
        else CLEAR_C();
        tmp >>= 1;

        CLEAR_N();
        UPDATE_Z_FLAG(cpu.rb.a);

        writeCPUByte(addrs, tmp);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x58: //cli
      {
        CLEAR_I();

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x59: // eor abs, y
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.y;
        cpu.rb.a ^= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
    case 0x5D: // eor abs, x
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        cpu.rb.a ^= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x5E: // lsr abs, x
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        uint8_t mem = readCPUByte(addrs);
        if (mem & BIT0) SET_C();
        else CLEAR_C();
        mem >>= 1;
        CLEAR_N();
        UPDATE_Z_FLAG(mem);
        writeCPUByte(addrs, mem);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x61: // adc x, ind
      {
        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte + cpu.rb.x);
        uint16_t addrs = readCPUByte(byte + cpu.rb.x + 1);
        addrs <<= 8;
        addrs |= low;
        uint8_t mem = readCPUByte(addrs);
        uint16_t sum = cpu.rb.a + mem + GET_C();

        if (~(cpu.rb.a ^ mem) & (cpu.rb.a ^ sum) & 0x80) SET_V();
        else CLEAR_V();
        if (sum & 0b100000000) SET_C();
        else CLEAR_C();

        cpu.rb.a = sum;
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x66: // ror zpg
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        uint8_t tmp = readCPUByte(addrs);
        
        uint8_t carry = (tmp & BIT0);
        tmp >>= 1;
        tmp |= GET_C() << 7;

        if (carry) SET_C();
        else CLEAR_C();

        UPDATE_N_FLAG(tmp);
        UPDATE_Z_FLAG(tmp);

        writeCPUByte(addrs, tmp);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x68: //pla impl
      {
        cpu.rb.sp++;
        uint16_t addrs = 0x0100 | cpu.rb.sp;
        cpu.rb.a = readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x69: // adc #
      {
        uint8_t imm = getInstructionByte();
        uint16_t sum = cpu.rb.a + imm + GET_C();

        if (~(cpu.rb.a ^ imm) & (cpu.rb.a ^ sum) & 0x80) SET_V();
        else CLEAR_V();
        if (sum & 0b100000000) SET_C();
        else CLEAR_C();
        cpu.rb.a = sum;

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x6A: // ror a
      {
        uint8_t carry = (cpu.rb.a & BIT0);
        cpu.rb.a >>= 1;
        cpu.rb.a |= GET_C() << 7;

        if (carry) SET_C();
        else CLEAR_C();
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x6C: // jmp (abs)
      {
        uint16_t addrs = getInstructionAddrs();
        cpu.rb.pc = readCPUByte(addrs) | (readCPUByte(addrs+1) << 8);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x6D: // adc abs
      {
        uint16_t addrs = getInstructionAddrs();
        uint8_t mem = readCPUByte(addrs);
        uint16_t sum = cpu.rb.a + mem + GET_C();

        if (~(cpu.rb.a ^ mem) & (cpu.rb.a ^ sum) & 0x80) SET_V();
        else CLEAR_V();
        if (sum & 0b100000000) SET_C();
        else CLEAR_C();
        cpu.rb.a = sum;

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x6E: // ror abs
      {
        uint16_t addrs = getInstructionAddrs();
        uint8_t mem = readCPUByte(addrs);
        uint8_t carry = mem & BIT0;
        mem >>= 1;
        mem |= GET_C() << 7;
        if (carry) SET_C();
        else CLEAR_C();
        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);
        writeCPUByte(addrs, mem);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x71: // adc ind, y
      {
        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte);
        uint16_t addrs = readCPUByte(byte +1);
        addrs <<= 8;
        addrs |= low;
        addrs += cpu.rb.y + GET_C();
        uint8_t mem = readCPUByte(addrs);
        uint16_t sum = cpu.rb.a + mem + GET_C();

        if (~(cpu.rb.a ^ mem) & (cpu.rb.a ^ sum) & 0x80) SET_V();
        else CLEAR_V();
        if (sum & 0b100000000) SET_C();
        else CLEAR_C();
        cpu.rb.a = sum;

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x76: // ror zpg, x
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        addrs += cpu.rb.x;
        uint8_t tmp = readCPUByte(addrs);
        
        uint8_t carry = (tmp & BIT0);
        tmp >>= 1;
        tmp |= GET_C() << 7;

        if (carry) SET_C();
        else CLEAR_C();

        UPDATE_N_FLAG(tmp);
        UPDATE_Z_FLAG(tmp);

        writeCPUByte(addrs, tmp);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x78: // sei
      {
        SET_I();

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x79: // adc abs, y
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.y;
        uint8_t mem = readCPUByte(addrs);
        uint16_t sum = cpu.rb.a + mem + GET_C();

        if(~(cpu.rb.a ^ mem) & (cpu.rb.a ^ sum) & 0x80) SET_V();
        else CLEAR_V();
        if (sum & 0b100000000) SET_C();
        else CLEAR_C();

        cpu.rb.a = sum;

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x7D: // adc abs, x
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        uint8_t mem = readCPUByte(addrs);
        uint16_t sum = cpu.rb.a + mem + GET_C();
        if (~(cpu.rb.a ^ mem) & (cpu.rb.a ^ sum) & 0x80) SET_V();
        else CLEAR_V();
        if (sum & 0b100000000) SET_C();
        else CLEAR_C();
        cpu.rb.a = sum;

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x7E: // ror abs, x
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        uint8_t mem = readCPUByte(addrs);
        uint8_t carry = mem & BIT0;
        mem >>= 1;
        mem |= GET_C() << 7;

        if (carry) SET_C();
        else CLEAR_C();
        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);

        writeCPUByte(addrs, mem);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x81: // sta x, ind
      {
        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte + cpu.rb.x);
        uint16_t addrs = readCPUByte(byte + cpu.rb.x + 1);
        addrs <<= 8;
        addrs |= low;
        writeCPUByte(addrs, cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x84: // sty zpg
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        writeCPUByte(addrs, cpu.rb.y);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x85: // sta zpg
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        writeCPUByte(addrs, cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x86: // stx zpg
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        writeCPUByte(addrs, cpu.rb.x);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x88: //dey
      {
        cpu.rb.y = cpu.rb.y - 1;

        UPDATE_N_FLAG(cpu.rb.y);
        UPDATE_Z_FLAG(cpu.rb.y);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;

    case 0x8A: // txa
      {
        cpu.rb.a = cpu.rb.x;

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x8C: // sty abs
      {
        uint16_t addrs = getInstructionAddrs();
        writeCPUByte(addrs, cpu.rb.y);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x8D: // sta abs
      {
        uint16_t addrs = getInstructionAddrs();
        writeCPUByte(addrs, cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x8E: // stx abs
      {
        uint16_t addrs = getInstructionAddrs();
        writeCPUByte(addrs, cpu.rb.x);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x91: // sta ind, y
      {
        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte);
        uint16_t addrs = readCPUByte(byte + 1);
        addrs <<= 8;
        addrs |= low;
        addrs += cpu.rb.y + GET_C();
        writeCPUByte(addrs, cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x94: // sty zpg, x
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        addrs += cpu.rb.x;
        writeCPUByte(addrs, cpu.rb.y);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x95: // sta zpg, x
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        addrs += cpu.rb.x;
        writeCPUByte(addrs, cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x96: // stx zpg, y
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        addrs += cpu.rb.y;
        writeCPUByte(addrs, cpu.rb.x);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x98: // tya
      {
        cpu.rb.a = cpu.rb.y;

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x99: // sta abs, y
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.y;
        writeCPUByte(addrs, cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0x9A: // txs
      {
        cpu.rb.sp = cpu.rb.x;

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0x9D: // sta abs, x
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        writeCPUByte(addrs, cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xA0: // ldy #imm
      {
        cpu.rb.y = getInstructionByte();

        UPDATE_N_FLAG(cpu.rb.x);
        UPDATE_Z_FLAG(cpu.rb.x);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0xA1: // lda x, ind
      {
        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte + cpu.rb.x);
        uint16_t addrs = readCPUByte(byte + cpu.rb.x + 1);
        addrs <<= 8;
        addrs |= low;
        cpu.rb.a = readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xA2: // ldx #imm
      {
        cpu.rb.x = getInstructionByte();

        UPDATE_Z_FLAG(cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.x);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0xA4: // ldy zpg
      {
        uint8_t addrs = getInstructionByte();
        cpu.rb.y = readCPUByte(addrs);

        UPDATE_Z_FLAG(cpu.rb.y);
        UPDATE_N_FLAG(cpu.rb.y);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xA5: // lda zpg
      {
        uint8_t addrs = getInstructionByte();
        cpu.rb.a = readCPUByte(addrs);

        UPDATE_Z_FLAG(cpu.rb.a);
        UPDATE_N_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xA6: // ldx zpg
      {
        uint8_t addrs = getInstructionByte();
        cpu.rb.x = readCPUByte(addrs);

        UPDATE_Z_FLAG(cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.x);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xA8: //tay
      {
        cpu.rb.y = cpu.rb.a;

        UPDATE_N_FLAG(cpu.rb.y);
        UPDATE_Z_FLAG(cpu.rb.y);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0xA9: // lda #
      {
        uint8_t imm = getInstructionByte();
        cpu.rb.a = imm;

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0xAA: //tax
      {
        cpu.rb.x = cpu.rb.a;

        UPDATE_N_FLAG(cpu.rb.x);
        UPDATE_Z_FLAG(cpu.rb.x);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0xAC: // ldy abs
      {
        uint16_t addrs = getInstructionAddrs();
        cpu.rb.y = readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.y);
        UPDATE_Z_FLAG(cpu.rb.y);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xAD: // lda abs
      {
        uint16_t addrs = getInstructionAddrs();
        cpu.rb.a = readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xAE: // ldx abs
      {
        uint16_t addrs = getInstructionAddrs();
        cpu.rb.x = readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.x);
        UPDATE_Z_FLAG(cpu.rb.x);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xB1: // lda ind, y
      {
        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte);
        uint16_t addrs = readCPUByte(byte + 1);
        addrs <<= 8;
        addrs |= low;
        addrs += cpu.rb.y + GET_C();
        cpu.rb.a = readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xB4: // ldy zpg, x
      {
        uint8_t addrs = getInstructionByte();
        addrs += cpu.rb.x;
        cpu.rb.y = readCPUByte(addrs);

        UPDATE_Z_FLAG(cpu.rb.y);
        UPDATE_N_FLAG(cpu.rb.y);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xB5: // lda zpg, x
      {
        uint8_t addrs = getInstructionByte();
        addrs += cpu.rb.x;
        cpu.rb.a = readCPUByte(addrs);

        UPDATE_Z_FLAG(cpu.rb.a);
        UPDATE_N_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xB6: // ldx zpg, y
      {
        uint8_t addrs = getInstructionByte();
        addrs += cpu.rb.y;
        cpu.rb.x = readCPUByte(addrs);

        UPDATE_Z_FLAG(cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.x);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xB8: //clv
      {
        CLEAR_V();

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0xB9: // lda abs, y
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.y;
        cpu.rb.a = readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xBA: //tsx
      {
        cpu.rb.x = cpu.rb.sp;

        UPDATE_N_FLAG(cpu.rb.x);
        UPDATE_Z_FLAG(cpu.rb.x);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0xBC: // ldy abs, x
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        cpu.rb.y = readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.y);
        UPDATE_Z_FLAG(cpu.rb.y);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xBD: // lda abs, x
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        cpu.rb.a = readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xBE: // ldx abs, y
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.y;
        cpu.rb.x = readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.x);
        UPDATE_Z_FLAG(cpu.rb.x);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xC1: // cmp x, ind
      {
        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte + cpu.rb.x);
        uint16_t addrs = readCPUByte(byte + cpu.rb.x + 1);
        addrs <<= 8;
        addrs |= low;
        uint8_t mem = readCPUByte(addrs);
        uint8_t sub = cpu.rb.a - mem;

        UPDATE_N_FLAG(sub);
        UPDATE_Z_FLAG(sub);
        if (cpu.rb.a >= mem) SET_C();
        else CLEAR_C();

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xC4: // cpy zpg
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        uint8_t mem = readCPUByte(addrs);
        uint8_t sub = cpu.rb.y - mem;

        UPDATE_Z_FLAG(sub);
        UPDATE_N_FLAG(sub);

        if (cpu.rb.y >= mem) SET_C();
        else CLEAR_C();

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xC5: // cmp zpg
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        uint8_t mem = readCPUByte(addrs);
        uint8_t sub = cpu.rb.a - mem;

        UPDATE_Z_FLAG(sub);
        UPDATE_N_FLAG(sub);

        if (cpu.rb.a >= mem) SET_C();
        else CLEAR_C();

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xC6: // dec zpg
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        uint8_t mem = readCPUByte(addrs) - 1;

        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);

        writeCPUByte(addrs, mem);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xC8: //iny
      {
        cpu.rb.y = cpu.rb.y + 1;

        UPDATE_N_FLAG(cpu.rb.y);
        UPDATE_Z_FLAG(cpu.rb.y);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0xC9: // cmp #
      {
        uint8_t imm = getInstructionByte();
        uint8_t sub = cpu.rb.a - imm;

        UPDATE_Z_FLAG(sub);
        UPDATE_N_FLAG(sub);

        if (cpu.rb.a >= imm) SET_C();
        else CLEAR_C();

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0xCA: //dex
      {
        cpu.rb.x = cpu.rb.x - 1;

        UPDATE_N_FLAG(cpu.rb.x);
        UPDATE_Z_FLAG(cpu.rb.x);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0xCC: // cpy abs
      {
        uint16_t addrs = getInstructionAddrs();
        uint8_t mem = readCPUByte(addrs);
        uint8_t sub = cpu.rb.y - mem;

        UPDATE_Z_FLAG(sub);
        UPDATE_N_FLAG(sub);

        if (cpu.rb.y >= mem) SET_C();
        else CLEAR_C();

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xCD: // cmp abs
      {
        uint16_t addrs = getInstructionAddrs();
        uint8_t mem = readCPUByte(addrs);
        uint8_t sub = cpu.rb.a - mem;

        UPDATE_Z_FLAG(sub);
        UPDATE_N_FLAG(sub);

        if (cpu.rb.a >= mem) SET_C();
        else CLEAR_C();

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xCE: // dec abs
      {
        uint16_t addrs = getInstructionAddrs();
        uint8_t mem = readCPUByte(addrs) - 1;

        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);
        writeCPUByte(addrs, mem);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xD1: // cmp ind, y
      {
        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte);
        uint16_t addrs = readCPUByte(byte + 1);
        addrs <<= 8;
        addrs |= low;
        addrs += cpu.rb.y + GET_C();
        uint8_t mem = readCPUByte(addrs);
        uint8_t sub = cpu.rb.a - mem;

        UPDATE_N_FLAG(sub);
        UPDATE_Z_FLAG(sub);

        if (cpu.rb.a >= mem) SET_C();
        else CLEAR_C();

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xD5: // cmp zpg, x
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        addrs += cpu.rb.x;
        uint8_t mem = readCPUByte(addrs);
        uint8_t sub = cpu.rb.a - mem;

        UPDATE_Z_FLAG(sub);
        UPDATE_N_FLAG(sub);

        if (cpu.rb.y >= mem) SET_C();
        else CLEAR_C();

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xD6: // dec zpg, x
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        addrs += cpu.rb.x;
        uint8_t mem = readCPUByte(addrs) - 1;

        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);

        writeCPUByte(addrs, mem);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xD8: // cld
      {
        CLEAR_D();

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0xD9: // cmp abs, y
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.y;
        uint8_t mem = readCPUByte(addrs);
        uint8_t sub = cpu.rb.a - mem;

        UPDATE_N_FLAG(sub);
        UPDATE_Z_FLAG(sub);

        if(cpu.rb.a >= mem) SET_C();
        else CLEAR_C();

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xDD: // cmp abs, x
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        uint8_t mem = readCPUByte(addrs);
        uint8_t sub = cpu.rb.a - mem;

        UPDATE_Z_FLAG(sub);
        UPDATE_N_FLAG(sub);

        if (cpu.rb.a >= mem) SET_C();
        else CLEAR_C();

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xDE: // dec abs, x
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        uint8_t mem = readCPUByte(addrs) - 1;

        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);
        writeCPUByte(addrs, mem);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xE1: // sbc x, ind
      {
        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte + cpu.rb.x);
        uint16_t addrs = readCPUByte(byte + cpu.rb.x + 1);
        addrs <<= 8;
        addrs |= low;
        uint8_t mem = ~readCPUByte(addrs);
        uint16_t sub = cpu.rb.a + mem + GET_C();

        if (~(cpu.rb.a ^ mem) & (cpu.rb.a ^ sub) & 0x80) SET_V();
        else CLEAR_V();
        if (sub & 0b100000000) SET_C();
        else CLEAR_C();
        cpu.rb.a = sub;
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xE4: // cpx zpg
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        uint8_t mem = readCPUByte(addrs);
        uint8_t sub = cpu.rb.x - mem;

        UPDATE_Z_FLAG(sub);
        UPDATE_N_FLAG(sub);

        if (cpu.rb.x >= mem) SET_C();
        else CLEAR_C();

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xE6: // inc zpg
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        uint8_t mem = readCPUByte(addrs) + 1;

        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);

        writeCPUByte(addrs, mem);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xE8: // inx
      {
        cpu.rb.x++;

        UPDATE_Z_FLAG(cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.x);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0xE9: // sbc #
      {
        uint8_t imm = ~getInstructionByte();
        uint16_t sub = cpu.rb.a + imm + GET_C();

        if (~(cpu.rb.a ^ imm) & (cpu.rb.a ^ sub) & 0x80) SET_V();
        else CLEAR_V();
        if (sub & 0b100000000) SET_C();
        else CLEAR_C();
        cpu.rb.a = sub;
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0xEA: //nop
      {
#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0xEC: // cpx abs
      {
        uint16_t addrs = getInstructionAddrs();
        uint8_t mem = readCPUByte(addrs);
        uint8_t sub = cpu.rb.x - mem;

        UPDATE_Z_FLAG(sub);
        UPDATE_N_FLAG(sub);

        if (cpu.rb.x >= mem) SET_C();
        else CLEAR_C();

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xED: // sbc abs
      {
        uint16_t addrs = getInstructionAddrs();
        uint8_t mem = ~readCPUByte(addrs);
        uint16_t sub = cpu.rb.a + mem + GET_C();

        if (~(cpu.rb.a ^ mem) & (cpu.rb.a ^ sub) & 0x80) SET_V();
        else CLEAR_V();
        if (sub & 0b100000000) SET_C();
        else CLEAR_C();
        cpu.rb.a = sub;
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xEE: // inc abs
      {
        uint16_t addrs = getInstructionAddrs();
        uint8_t mem = readCPUByte(addrs) + 1;

        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);

        writeCPUByte(addrs, mem);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xF1: // sbc ind, y
      {
        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte);
        uint16_t addrs = readCPUByte(byte + 1);
        addrs <<= 8;
        addrs |= low;
        addrs += cpu.rb.y + GET_C();
        uint8_t mem = ~readCPUByte(addrs);
        uint16_t sub = cpu.rb.a + mem + GET_C();

        if (~(cpu.rb.a ^ mem) & (cpu.rb.a ^ sub) & 0x80) SET_V();
        else CLEAR_V();
        if (sub & 0b100000000) SET_C();
        else CLEAR_C();
        cpu.rb.a = sub;
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xF6: // inc zpg, x
      {
        uint16_t addrs = getInstructionByte(); // highest 8 bits are 0
        addrs += cpu.rb.x;
        uint8_t mem = readCPUByte(addrs) + 1;

        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);

        writeCPUByte(addrs, mem);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xF8: //sed
      {
        SET_D();

#ifdef DEBUG_PRINT
        print(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p);
#endif
      }
      break;
    case 0xF9: //sbc abs, y
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.y;
        uint8_t mem = ~readCPUByte(addrs);
        uint16_t sub = cpu.rb.a + mem + GET_C();

        if (~(cpu.rb.a ^ mem) & (cpu.rb.a ^ sub) & 0x80) SET_V();
        else CLEAR_V();
        if (sub & 0b100000000) SET_C();
        else CLEAR_C();
        cpu.rb.a = sub;
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xFD: // sbc abs, x
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        uint8_t mem = ~readCPUByte(addrs);
        uint16_t sub = cpu.rb.a + mem + GET_C();

        if (~(cpu.rb.a ^ mem) & (cpu.rb.a ^ sub) & 0x80) SET_V();
        else CLEAR_V();
        if (sub & 0b100000000) SET_C();
        else CLEAR_C();
        cpu.rb.a = sub;
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    case 0xFE: // inc abs, x
      {
        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        uint8_t mem = readCPUByte(addrs) + 1;

        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);

        writeCPUByte(addrs, mem);

#ifdef DEBUG_PRINT
        printls(cpu.rb.a, cpu.rb.x, cpu.rb.y, cpu.rb.sp, cpu.rb.pc, cpu.rb.p, addrs, readCPUByte(addrs));
#endif
      }
      break;
    default:
#ifdef DEBUG_PRINT
      fprintf(stderr, COLOR_RED "Unimplemented OP Code: 0x%02X\n", opcode);
#endif
      exit(1);
  }
}
