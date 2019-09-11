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
    case 0x0D: // ora abs
      {
        cpu.rb.a |= readCPUByte(getInstructionAddrs());
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
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
      }
      break;
    case 0x10: // bpl dest
      {
        uint8_t dest = getInstructionByte();
        if (!GET_N()) ADD_SIGNED_TO_UNSIGNED(dest, cpu.rb.pc);
      }
      break;
    case 0x1D: // ora abs, x
      {
        cpu.rb.a |= readCPUByte(getInstructionAddrs() + cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
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
      }
      break;
    case 0x2C: // bit abs
      {
        uint8_t mem = readCPUByte(getInstructionAddrs());
        cpu.rb.p = (cpu.rb.p & ~(BIT7|BIT6)) | (mem & (BIT7|BIT6));
        UPDATE_Z_FLAG(cpu.rb.a & mem);
      }
      break;
    case 0x2D: // and abs
      {
        cpu.rb.a &= readCPUByte(getInstructionAddrs());
        UPDATE_Z_FLAG(cpu.rb.a);
        UPDATE_N_FLAG(cpu.rb.a);
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
      }
      break;
    case 0x3D: // and abs, x
      {
        cpu.rb.a &= readCPUByte(getInstructionAddrs() + cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
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
      }
      break;
    case 0x4C: // jmp abs
      {
        cpu.rb.pc = getInstructionAddrs();
      }
      break;
    case 0x4D: // eor abs
      {
        cpu.rb.a ^= readCPUByte(getInstructionAddrs());
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
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
      }
      break;
    case 0x5D: // eor abs, x
      {
        cpu.rb.a ^= readCPUByte(getInstructionAddrs() + cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
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
      }
      break;
    case 0x6C: // jmp (abs)
      {
        uint16_t addrs = getInstructionAddrs();
        cpu.rb.pc = readCPUByte(addrs) | (readCPUByte(addrs+1) << 8);
      }
      break;
    case 0x6D: // adc abs
      {
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
    case 0x78: // sei
      {
        SET_I();
      }
      break;
    case 0x7D: // adc abs, x
      {
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
    case 0x8C: // sty abs
      {
        writeCPUByte(getInstructionAddrs(), cpu.rb.y);
      }
      break;
    case 0x8D: // sta abs
      {
        writeCPUByte(getInstructionAddrs(), cpu.rb.a);
      }
      break;
    case 0x8E: // stx abs
      {
        writeCPUByte(getInstructionAddrs(), cpu.rb.x);
      }
      break;
    case 0x9A: // txs
      {
        cpu.rb.sp = cpu.rb.x;
      }
      break;
    case 0x9D: // sta abs, x
      {
        writeCPUByte(getInstructionAddrs() + cpu.rb.x, cpu.rb.a);
      }
      break;
    case 0xA2: // ldx #imm
      {
        cpu.rb.x = getInstructionByte();

        UPDATE_Z_FLAG(cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.x);
      }
      break;
    case 0xAC: // ldy abs
      {
        cpu.rb.y = readCPUByte(getInstructionAddrs());
        UPDATE_N_FLAG(cpu.rb.y);
        UPDATE_Z_FLAG(cpu.rb.y);
      }
      break;
    case 0xAD: // lda abs
      {
        cpu.rb.a = readCPUByte(getInstructionAddrs());
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0xAE: // ldx abs
      {
        cpu.rb.x = readCPUByte(getInstructionAddrs());
        UPDATE_N_FLAG(cpu.rb.x);
        UPDATE_Z_FLAG(cpu.rb.x);
      }
      break;
    case 0xBC: // ldy abs, x
      {
        cpu.rb.y = readCPUByte(getInstructionAddrs() + cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.y);
        UPDATE_Z_FLAG(cpu.rb.y);
      }
      break;
    case 0xBD: // lda abs, x
      {
        cpu.rb.a = readCPUByte(getInstructionAddrs() + cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.a);
        UPDATE_Z_FLAG(cpu.rb.a);
      }
      break;
    case 0xBE: // ldx abs, y
      {
        cpu.rb.x = readCPUByte(getInstructionAddrs() + cpu.rb.y);
        UPDATE_N_FLAG(cpu.rb.x);
        UPDATE_Z_FLAG(cpu.rb.x);
      }
      break;
    case 0xCC: // cpy abs
      {
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
        uint16_t addrs = getInstructionAddrs();
        uint8_t mem = readCPUByte(addrs) - 1;
        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);
        writeCPUByte(addrs, mem);
      }
      break;
    case 0xD8: // cld
      {
        CLEAR_D();
      }
      break;
    case 0xDD: // cmp abs, x
      {
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
        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        uint8_t mem = readCPUByte(addrs) - 1;
        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);
        writeCPUByte(addrs, mem);
      }
      break;
    case 0xE8: // inx
      {
        cpu.rb.x++;

        UPDATE_Z_FLAG(cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.x);
      }
      break;
    case 0xEC: // cpx abs
      {
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
        uint16_t addrs = getInstructionAddrs();
        uint8_t mem = readCPUByte(addrs) + 1;
        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);
        writeCPUByte(addrs, mem);
      }
      break;
    case 0xFD: // sbc abs, x
      {
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
        uint16_t addrs = getInstructionAddrs() + cpu.rb.x;
        uint8_t mem = readCPUByte(addrs) + 1;
        UPDATE_N_FLAG(mem);
        UPDATE_Z_FLAG(mem);
        writeCPUByte(addrs, mem);
      }
      break;
    default:
      fprintf(stderr, COLOR_RED "Unimplemented OP Code: 0x%02X\n", opcode);
      exit(1);
  }
}
