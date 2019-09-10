#define SET_C() (cpu.rb.p |= BIT0)
#define CLEAR_C() (cpu.rb.p &= ~BIT0)
#define GET_C() (cpu.rb.p | BIT0)

#define SET_Z() (cpu.rb.p |= BIT1)
#define CLEAR_Z() (cpu.rb.p &= ~BIT1)
#define GET_Z() (cpu.rb.p | BIT1)

#define SET_I() (cpu.rb.p |= BIT2)
#define CLEAR_I() (cpu.rb.p &= ~BIT2)
#define GET_I() (cpu.rb.p | BIT2)

#define SET_D() (cpu.rb.p |= BIT3)
#define CLEAR_D() (cpu.rb.p &= ~BIT3)
#define GET_D() (cpu.rb.p | BIT3)

#define SET_V() (cpu.rb.p |= BIT6)
#define CLEAR_V() (cpu.rb.p &= ~BIT6)
#define GET_V() (cpu.rb.p | BIT6)

#define SET_N() (cpu.rb.p |= BIT7)
#define CLEAR_N() (cpu.rb.p &= ~BIT7)
#define GET_N() (cpu.rb.p | BIT7)

#define ADD_SIGNED_TO_UNSIGNED(a, b) { \
  if (a < 128) b += a;                 \
  else b -= (~a + 1);                  \
}

#define UPDATE_Z_FLAG(a) { \
  if (a) CLEAR_Z();        \
  else SET_Z();            \
}

#define UPDATE_N_FLAG(a) { \
  if (a < 128) CLEAR_N();  \
  else SET_N();            \
}

void doInstruction(uint8_t opcode) {
  switch (opcode) {
    case 0x10: // bpl dest
      {
        uint8_t dest = getInstructionByte();
        if (!GET_N()) ADD_SIGNED_TO_UNSIGNED(dest, cpu.rb.pc);
      }
    case 0x2C: // bit abs
      {
        uint8_t mem = readCPUByte(getInstructionAddrs());
        cpu.rb.p = (cpu.rb.p & ~(BIT7|BIT6)) | (mem & (BIT7|BIT6));
        UPDATE_Z_FLAG(cpu.rb.a & mem);
      }
      break;
    case 0x78: // sei
      {
        SET_I();
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
    case 0xA2: // ldx #imm
      {
        cpu.rb.x = getInstructionByte();

        UPDATE_Z_FLAG(cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.x);
      }
      break;
    case 0xD8: // cld
      {
        CLEAR_D();
      }
      break;
    case 0xE8: // inx
      {
        cpu.rb.x++;

        UPDATE_Z_FLAG(cpu.rb.x);
        UPDATE_N_FLAG(cpu.rb.x);
      }
      break;
    default:
      fprintf(stderr, COLOR_RED "Unimplemented OP Code: 0x%X\n", opcode);
      exit(1);
  }
}
