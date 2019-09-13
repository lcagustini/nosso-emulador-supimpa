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
#ifdef DEBUG_PRINT
  printf(COLOR_BLUE);
#endif
  switch (opcode) {
    case 0x01: // ora x, ind
      {
#ifdef DEBUG_PRINT
        printf("ora x, ind\n");
#endif

        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte + cpu.rb.x);
        uint16_t addrs = readCPUByte(byte + cpu.rb.x + 1);
        addrs <<= 8;
        addrs |= low;
        cpu.rb.a |= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x08: //php impl
      {
#ifdef DEBUG_PRINT
        printf("php impl\n");
#endif
        uint16_t addrs = 0x0100 | cpu.rb.sp;
        writeCPUByte(addrs, cpu.rb.p);
        cpu.rb.sp--;
      }
      break;
    case 0x0A: // asl a
      {
#ifdef DEBUG_PRINT
        printf("asl a\n");
#endif
        if (cpu.rb.a & BIT7) SET_C();
        else CLEAR_C();
        cpu.rb.a <<= 1;
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x0D: // ora abs
      {
#ifdef DEBUG_PRINT
        printf("ora abs\n");
#endif

        cpu.rb.a |= readCPUByte(getInstructionAddrs());
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x0E: // asl abs
      {
#ifdef DEBUG_PRINT
        printf("asl abs\n");
#endif

        uint16_t addrs = getInstructionAddrs();
        uint8_t mem = readCPUByte(addrs);
        if (mem & BIT7) SET_C();
        else CLEAR_C();
        mem <<= 1;
        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);
        writeCPUByte(addrs, mem);
      }
      break;
    case 0x10: // bpl dest
      {
#ifdef DEBUG_PRINT
        printf("bpl dest\n");
#endif

        uint8_t dest = getInstructionByte();
        if (!GET_N()) ADD_SIGNED_TO_UNSIGNED(dest, cpu.rb.pc);
      }
      break;
    case 0x11: // ora ind, y
      {
#ifdef DEBUG_PRINT
        printf("ora ind, y\n");
#endif

        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte);
        uint16_t addrs = readCPUByte(byte + 1);
        addrs <<= 8;
        addrs |= low;
        addrs += cpu.rb.y + GET_C();
        cpu.rb.a |= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x18: //clc impl
      {
#ifdef DEBUG_PRINT
        printf("clc impl\n");
#endif
        CLEAR_C();
      }
      break;
    case 0x1D: // ora abs, x
      {
#ifdef DEBUG_PRINT
        printf("ora abs, x\n");
#endif

        cpu.rb.a |= readCPUByte(getInstructionAddrs() + cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x1E: // asl abs, x
      {
#ifdef DEBUG_PRINT
        printf("asl abs, x\n");
#endif

        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        uint8_t mem = readCPUByte(addrs);
        if (mem & BIT7) SET_C();
        else CLEAR_C();
        mem <<= 1;
        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);
        writeCPUByte(addrs, mem);
      }
      break;
    case 0x21: // and x, ind
      {
#ifdef DEBUG_PRINT
        printf("and x, ind\n");
#endif

        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte + cpu.rb.x);
        uint16_t addrs = readCPUByte(byte + cpu.rb.x + 1);
        addrs <<= 8;
        addrs |= low;
        cpu.rb.a &= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x28: //plp impl
      {
#ifdef DEBUG_PRINT
        printf("plp impl\n");
#endif
        cpu.rb.sp++;
        uint16_t addrs = 0x0100 | cpu.rb.sp;
        cpu.rb.p = readCPUByte(addrs);
      }
      break;
    case 0x2A: // rol a
      {
#ifdef DEBUG_PRINT
        printf("rol a\n");
#endif

        uint8_t carry = cpu.rb.a & BIT7;
        cpu.rb.a <<= 1;
        cpu.rb.a |= GET_C();
        if (carry) SET_C();
        else CLEAR_C();
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x2C: // bit abs
      {
#ifdef DEBUG_PRINT
        printf("bit abs\n");
#endif

        uint8_t mem = readCPUByte(getInstructionAddrs());
        cpu.rb.p = (cpu.rb.p & ~(BIT7|BIT6)) | (mem & (BIT7|BIT6));
        UPDATE_Z_FLAG(cpu.rb.a & mem);
      }
      break;
    case 0x2D: // and abs
      {
#ifdef DEBUG_PRINT
        printf("and abs\n");
#endif

        cpu.rb.a &= readCPUByte(getInstructionAddrs());
        UPDATE_Z_FLAG(cpu.rb.a);
        UPDATE_N_FLAG(cpu.rb.a);
      }
      break;
    case 0x2E: // rol abs
      {
#ifdef DEBUG_PRINT
        printf("rol abs\n");
#endif

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
      }
      break;
    case 0x31: // and ind, y
      {
#ifdef DEBUG_PRINT
        printf("and ind, y\n");
#endif

        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte);
        uint16_t addrs = readCPUByte(byte + 1);
        addrs <<= 8;
        addrs |= low;
        addrs += cpu.rb.y + GET_C();
        cpu.rb.a &= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x38: //sec
      {
#ifdef DEBUG_PRINT
        printf("sec\n");
#endif

        SET_C();
      }
      break;
    case 0x3D: // and abs, x
      {
#ifdef DEBUG_PRINT
        printf("and abs, x\n");
#endif

        cpu.rb.a &= readCPUByte(getInstructionAddrs() + cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x3E: // rol abs, x
      {
#ifdef DEBUG_PRINT
        printf("rol abs, x\n");
#endif

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
      }
      break;
    case 0x41: // eor x, ind
      {
#ifdef DEBUG_PRINT
        printf("eor x, ind\n");
#endif

        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte + cpu.rb.x);
        uint16_t addrs = readCPUByte(byte + cpu.rb.x + 1);
        addrs <<= 8;
        addrs |= low;
        cpu.rb.a ^= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x48: //pha impl
      {
#ifdef DEBUG_PRINT
        printf("pha impl\n");
#endif
        uint16_t addrs = 0x0100 | cpu.rb.sp;
        writeCPUByte(addrs, cpu.rb.a);
        cpu.rb.sp--;
      }
      break;
    case 0x4C: // jmp abs
      {
#ifdef DEBUG_PRINT
        printf("jmp abs\n");
#endif

        cpu.rb.pc = getInstructionAddrs();
      }
      break;
    case 0x4A: // lsr a
      {
#ifdef DEBUG_PRINT
        printf("lsr a\n");
#endif

        if (cpu.rb.a & BIT0) SET_C();
        else CLEAR_C();
        cpu.rb.a >>= 1;
        CLEAR_N();
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x4D: // eor abs
      {
#ifdef DEBUG_PRINT
        printf("eor abs\n");
#endif

        cpu.rb.a ^= readCPUByte(getInstructionAddrs());
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x4E: // lsr abs
      {
#ifdef DEBUG_PRINT
        printf("lsr abs\n");
#endif

        uint16_t addrs = getInstructionAddrs();
        uint8_t mem = readCPUByte(addrs);
        if (mem & BIT0) SET_C();
        else CLEAR_C();
        mem >>= 1;
        CLEAR_N();
        UPDATE_Z_FLAG(mem);
        writeCPUByte(addrs, mem);
      }
      break;
    case 0x51: // eor ind, y
      {
#ifdef DEBUG_PRINT
        printf("eor ind, y\n");
#endif

        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte);
        uint16_t addrs = readCPUByte(byte + 1);
        addrs <<= 8;
        addrs |= low;
        addrs += cpu.rb.y + GET_C();
        cpu.rb.a ^= readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x58: //cli
      {
#ifdef DEBUG_PRINT
        printf("cli\n");
#endif

        CLEAR_I();
      }
      break;
    case 0x5D: // eor abs, x
      {
#ifdef DEBUG_PRINT
        printf("eor abs, x\n");
#endif

        cpu.rb.a ^= readCPUByte(getInstructionAddrs() + cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x5E: // lsr abs, x
      {
#ifdef DEBUG_PRINT
        printf("lsr abs, x\n");
#endif

        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        uint8_t mem = readCPUByte(addrs);
        if (mem & BIT0) SET_C();
        else CLEAR_C();
        mem >>= 1;
        CLEAR_N();
        UPDATE_Z_FLAG(mem);
        writeCPUByte(addrs, mem);
      }
      break;
    case 0x61: // adc x, ind
      {
#ifdef DEBUG_PRINT
        printf("adc x, ind\n");
#endif

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
      }
      break;
    case 0x68: //pla impl
      {
#ifdef DEBUG_PRINT
        printf("pla impl\n");
#endif
        cpu.rb.sp++;
        uint16_t addrs = 0x0100 | cpu.rb.sp;
        cpu.rb.a = readCPUByte(addrs);
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x6A: // ror a
      {
#ifdef DEBUG_PRINT
        printf("ror a\n");
#endif
        uint8_t carry = (cpu.rb.a & BIT0);
        cpu.rb.a >>= 1;
        cpu.rb.a |= GET_C() << 7;
        if (carry) SET_C();
        else CLEAR_C();
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x6C: // jmp (abs)
      {
#ifdef DEBUG_PRINT
        printf("jmp (abs)\n");
#endif

        uint16_t addrs = getInstructionAddrs();
        cpu.rb.pc = readCPUByte(addrs) | (readCPUByte(addrs+1) << 8);
      }
      break;
    case 0x6D: // adc abs
      {
#ifdef DEBUG_PRINT
        printf("adc abs\n");
#endif

        uint8_t mem = readCPUByte(getInstructionAddrs());
        uint16_t sum = cpu.rb.a + mem + GET_C();
        if (~(cpu.rb.a ^ mem) & (cpu.rb.a ^ sum) & 0x80) SET_V();
        else CLEAR_V();
        if (sum & 0b100000000) SET_C();
        else CLEAR_C();
        cpu.rb.a = sum;
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x6E: // ror abs
      {
#ifdef DEBUG_PRINT
        printf("ror abs\n");
#endif

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
      }
      break;
    case 0x71: // adc ind, y
      {
#ifdef DEBUG_PRINT
        printf("adc ind, y\n");
#endif

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
      }
      break;
    case 0x78: // sei
      {
#ifdef DEBUG_PRINT
        printf("sei\n");
#endif

        SET_I();
      }
      break;
    case 0x7D: // adc abs, x
      {
#ifdef DEBUG_PRINT
        printf("adc abs, x\n");
#endif

        uint8_t mem = readCPUByte(getInstructionAddrs() + cpu.rb.x);
        uint16_t sum = cpu.rb.a + mem + GET_C();
        if (~(cpu.rb.a ^ mem) & (cpu.rb.a ^ sum) & 0x80) SET_V();
        else CLEAR_V();
        if (sum & 0b100000000) SET_C();
        else CLEAR_C();
        cpu.rb.a = sum;
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x7E: // ror abs, x
      {
#ifdef DEBUG_PRINT
        printf("ror abs, x\n");
#endif

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
      }
      break;
    case 0x81: // sta x, ind
      {
#ifdef DEBUG_PRINT
        printf("sta x, ind\n");
#endif

        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte + cpu.rb.x);
        uint16_t addrs = readCPUByte(byte + cpu.rb.x + 1);
        addrs <<= 8;
        addrs |= low;
        writeCPUByte(addrs, cpu.rb.a);
      }
      break;
    case 0x88: //dey
      {
#ifdef DEBUG_PRINT
        printf("dey\n");
#endif

        cpu.rb.y = cpu.rb.y - 1;
        UPDATE_N_FLAG(cpu.rb.y);
        UPDATE_Z_FLAG(cpu.rb.y);
      }
      break;
    case 0x8A: // txa
      {
#ifdef DEBUG_PRINT
        printf("txa\n");
#endif

        cpu.rb.a = cpu.rb.x;
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x8C: // sty abs
      {
#ifdef DEBUG_PRINT
        printf("sty abs\n");
#endif

        writeCPUByte(getInstructionAddrs(), cpu.rb.y);
      }
      break;
    case 0x8D: // sta abs
      {
#ifdef DEBUG_PRINT
        printf("sta abs\n");
#endif

        writeCPUByte(getInstructionAddrs(), cpu.rb.a);
      }
      break;
    case 0x8E: // stx abs
      {
#ifdef DEBUG_PRINT
        printf("stx abs\n");
#endif

        writeCPUByte(getInstructionAddrs(), cpu.rb.x);
      }
      break;
    case 0x91: // sta ind, y
      {
#ifdef DEBUG_PRINT
        printf("sta ind, y\n");
#endif

        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte);
        uint16_t addrs = readCPUByte(byte + 1);
        addrs <<= 8;
        addrs |= low;
        addrs += cpu.rb.y + GET_C();
        writeCPUByte(addrs, cpu.rb.a);
      }
      break;
    case 0x98: // tya
      {
#ifdef DEBUG_PRINT
        printf("tya\n");
#endif

        cpu.rb.a = cpu.rb.y;
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0x9A: // txs
      {
#ifdef DEBUG_PRINT
        printf("txs\n");
#endif

        cpu.rb.sp = cpu.rb.x;
      }
      break;
    case 0x9D: // sta abs, x
      {
#ifdef DEBUG_PRINT
        printf("sta abs, x\n");
#endif

        writeCPUByte(getInstructionAddrs() + cpu.rb.x, cpu.rb.a);
      }
      break;
    case 0xA0: // ldy #imm
      {
#ifdef DEBUG_PRINT
        printf("ldy #imm\n");
#endif

        cpu.rb.y = getInstructionByte();
        UPDATE_N_FLAG(cpu.rb.x);
        UPDATE_Z_FLAG(cpu.rb.x);
      }
      break;
    case 0xA1: // lda x, ind
      {
#ifdef DEBUG_PRINT
        printf("lda x, ind\n");
#endif

        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte + cpu.rb.x);
        uint16_t addrs = readCPUByte(byte + cpu.rb.x + 1);
        addrs <<= 8;
        addrs |= low;
        cpu.rb.a = readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0xA2: // ldx #imm
      {
#ifdef DEBUG_PRINT
        printf("ldx #imm\n");
#endif

        cpu.rb.x = getInstructionByte();

        UPDATE_Z_FLAG(cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.x);
      }
      break;
    case 0xA8: //tay
      {
#ifdef DEBUG_PRINT
        printf("tay\n");
#endif

        cpu.rb.y = cpu.rb.a;
        UPDATE_N_FLAG(cpu.rb.y);
        UPDATE_Z_FLAG(cpu.rb.y);
      }
      break;
    case 0xAA: //tax
      {
#ifdef DEBUG_PRINT
        printf("tax\n");
#endif

        cpu.rb.x = cpu.rb.a;
        UPDATE_N_FLAG(cpu.rb.x);
        UPDATE_Z_FLAG(cpu.rb.x);
      }
      break;
    case 0xAC: // ldy abs
      {
#ifdef DEBUG_PRINT
        printf("ldy abs\n");
#endif

        cpu.rb.y = readCPUByte(getInstructionAddrs());
        UPDATE_N_FLAG(cpu.rb.y);
        UPDATE_Z_FLAG(cpu.rb.y);
      }
      break;
    case 0xAD: // lda abs
      {
#ifdef DEBUG_PRINT
        printf("lda abs\n");
#endif

        cpu.rb.a = readCPUByte(getInstructionAddrs());
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0xAE: // ldx abs
      {
#ifdef DEBUG_PRINT
        printf("ldx abs\n");
#endif

        cpu.rb.x = readCPUByte(getInstructionAddrs());
        UPDATE_N_FLAG(cpu.rb.x);
        UPDATE_Z_FLAG(cpu.rb.x);
      }
      break;
    case 0xB1: // lda ind, y
      {
#ifdef DEBUG_PRINT
        printf("lda ind, y\n");
#endif

        uint8_t byte = getInstructionByte();
        uint8_t low = readCPUByte(byte);
        uint16_t addrs = readCPUByte(byte + 1);
        addrs <<= 8;
        addrs |= low;
        addrs += cpu.rb.y + GET_C();
        cpu.rb.a = readCPUByte(addrs);

        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0xB8: //clv
      {
#ifdef DEBUG_PRINT
        printf("clv\n");
#endif

        CLEAR_V();
      }
      break;
    case 0xBA: //tsx
      {
#ifdef DEBUG_PRINT
        printf("tsx\n");
#endif

        cpu.rb.x = cpu.rb.sp;
        UPDATE_N_FLAG(cpu.rb.x);
        UPDATE_Z_FLAG(cpu.rb.x);
      }
      break;
    case 0xBC: // ldy abs, x
      {
#ifdef DEBUG_PRINT
        printf("ldy abs, x\n");
#endif

        cpu.rb.y = readCPUByte(getInstructionAddrs() + cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.y);
        UPDATE_Z_FLAG(cpu.rb.y);
      }
      break;
    case 0xBD: // lda abs, x
      {
#ifdef DEBUG_PRINT
        printf("lda abs, x\n");
#endif

        cpu.rb.a = readCPUByte(getInstructionAddrs() + cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0xBE: // ldx abs, y
      {
#ifdef DEBUG_PRINT
        printf("ldx abs, y\n");
#endif

        cpu.rb.x = readCPUByte(getInstructionAddrs() + cpu.rb.y);
        UPDATE_N_FLAG(cpu.rb.x);
        UPDATE_Z_FLAG(cpu.rb.x);
      }
      break;
    case 0xC1: // cmp x, ind
      {
#ifdef DEBUG_PRINT
        printf("cmp x, ind\n");
#endif

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
      }
      break;
    case 0xC8: //iny
      {
#ifdef DEBUG_PRINT
        printf("iny\n");
#endif

        cpu.rb.y = cpu.rb.y + 1;
        UPDATE_N_FLAG(cpu.rb.y);
        UPDATE_Z_FLAG(cpu.rb.y);
      }
      break;
    case 0xCA: //dex
      {
#ifdef DEBUG_PRINT
        printf("dex\n");
#endif

        cpu.rb.x = cpu.rb.x - 1;
        UPDATE_N_FLAG(cpu.rb.x);
        UPDATE_Z_FLAG(cpu.rb.x);
      }
      break;
    case 0xCC: // cpy abs
      {
#ifdef DEBUG_PRINT
        printf("cpy abs\n");
#endif

        uint8_t mem = readCPUByte(getInstructionAddrs());
        uint8_t sub = cpu.rb.y - mem;

        UPDATE_Z_FLAG(sub);
        UPDATE_N_FLAG(sub);

        if (cpu.rb.y >= mem) SET_C();
        else CLEAR_C();
      }
      break;
    case 0xCD: // cmp abs
      {
#ifdef DEBUG_PRINT
        printf("cmp abs\n");
#endif

        uint8_t mem = readCPUByte(getInstructionAddrs());
        uint8_t sub = cpu.rb.a - mem;

        UPDATE_Z_FLAG(sub);
        UPDATE_N_FLAG(sub);

        if (cpu.rb.a >= mem) SET_C();
        else CLEAR_C();
      }
      break;
    case 0xCE: // dec abs
      {
#ifdef DEBUG_PRINT
        printf("dec abs\n");
#endif

        uint16_t addrs = getInstructionAddrs();
        uint8_t mem = readCPUByte(addrs) - 1;
        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);
        writeCPUByte(addrs, mem);
      }
      break;
    case 0xD1: // cmp ind, y
      {
#ifdef DEBUG_PRINT
        printf("cmp ind, y\n");
#endif

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
      }
      break;
    case 0xD8: // cld
      {
#ifdef DEBUG_PRINT
        printf("cld\n");
#endif

        CLEAR_D();
      }
      break;
    case 0xDD: // cmp abs, x
      {
#ifdef DEBUG_PRINT
        printf("cmp abs, x\n");
#endif

        uint8_t mem = readCPUByte(getInstructionAddrs() + cpu.rb.x);
        uint8_t sub = cpu.rb.a - mem;

        UPDATE_Z_FLAG(sub);
        UPDATE_N_FLAG(sub);

        if (cpu.rb.a >= mem) SET_C();
        else CLEAR_C();
      }
      break;
    case 0xDE: // dec abs, x
      {
#ifdef DEBUG_PRINT
        printf("dec abs, x\n");
#endif

        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        uint8_t mem = readCPUByte(addrs) - 1;
        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);
        writeCPUByte(addrs, mem);
      }
      break;
    case 0xE1: // sbc x, ind
      {
#ifdef DEBUG_PRINT
        printf("sbc x, ind\n");
#endif

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
      }
      break;
    case 0xE8: // inx
      {
#ifdef DEBUG_PRINT
        printf("inx\n");
#endif

        cpu.rb.x++;

        UPDATE_Z_FLAG(cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.x);
      }
      break;
    case 0xEA: //nop
      {
#ifdef DEBUG_PRINT
        printf("nop\n");
#endif
      }
      break;
    case 0xEC: // cpx abs
      {
#ifdef DEBUG_PRINT
        printf("cpx abs\n");
#endif

        uint8_t mem = readCPUByte(getInstructionAddrs());
        uint8_t sub = cpu.rb.x - mem;

        UPDATE_Z_FLAG(sub);
        UPDATE_N_FLAG(sub);

        if (cpu.rb.x >= mem) SET_C();
        else CLEAR_C();
      }
      break;
    case 0xED: // sbc abs
      {
#ifdef DEBUG_PRINT
        printf("sbc abs\n");
#endif

        uint8_t mem = ~readCPUByte(getInstructionAddrs());
        uint16_t sub = cpu.rb.a + mem + GET_C();
        if (~(cpu.rb.a ^ mem) & (cpu.rb.a ^ sub) & 0x80) SET_V();
        else CLEAR_V();
        if (sub & 0b100000000) SET_C();
        else CLEAR_C();
        cpu.rb.a = sub;
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0xEE: // inc abs
      {
#ifdef DEBUG_PRINT
        printf("inc abs\n");
#endif

        uint16_t addrs = getInstructionAddrs();
        uint8_t mem = readCPUByte(addrs) + 1;
        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);
        writeCPUByte(addrs, mem);
      }
      break;
    case 0xF1: // sbc ind, y
      {
#ifdef DEBUG_PRINT
        printf("sbc ind, y\n");
#endif

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
      }
      break;
    case 0xF8: //sed
      {
#ifdef DEBUG_PRINT
        printf("sed\n");
#endif

        SET_D();
      }
      break;
    case 0xFD: // sbc abs, x
      {
#ifdef DEBUG_PRINT
        printf("sbc abs, x\n");
#endif

        uint8_t mem = ~readCPUByte(getInstructionAddrs() + cpu.rb.x);
        uint16_t sub = cpu.rb.a + mem + GET_C();
        if (~(cpu.rb.a ^ mem) & (cpu.rb.a ^ sub) & 0x80) SET_V();
        else CLEAR_V();
        if (sub & 0b100000000) SET_C();
        else CLEAR_C();
        cpu.rb.a = sub;
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0xFE: // inc abs, x
      {
#ifdef DEBUG_PRINT
        printf("inc abs, x\n");
#endif

        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        uint8_t mem = readCPUByte(addrs) + 1;
        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);
        writeCPUByte(addrs, mem);
      }
      break;
    default:
#ifdef DEBUG_PRINT
      fprintf(stderr, COLOR_RED "Unimplemented OP Code: 0x%02X\n", opcode);
#endif
      exit(1);
  }
}
