#include "cpu_decoder.h"

CPUInstruction cpu_decode(uint8_t opcode) {
  switch (opcode) {
  case 0x69:
    return (CPUInstruction){ADC, IMMEDIATE};
  case 0x65:
    return (CPUInstruction){ADC, ZERO_PAGE};
  case 0x75:
    return (CPUInstruction){ADC, ZERO_PAGE_X};
  case 0x6D:
    return (CPUInstruction){ADC, ABSOLUTE};
  case 0x7D:
    return (CPUInstruction){ADC, ABSOLUTE_X_WITH_PENALTY};
  case 0x79:
    return (CPUInstruction){ADC, ABSOLUTE_Y_WITH_PENALTY};
  case 0x61:
    return (CPUInstruction){ADC, INDEXED_INDIRECT};
  case 0x71:
    return (CPUInstruction){ADC, INDIRECT_INDEXED_WITH_PENALTY};

  case 0x29:
    return (CPUInstruction){AND, IMMEDIATE};
  case 0x25:
    return (CPUInstruction){AND, ZERO_PAGE};
  case 0x35:
    return (CPUInstruction){AND, ZERO_PAGE_X};
  case 0x2D:
    return (CPUInstruction){AND, ABSOLUTE};
  case 0x3D:
    return (CPUInstruction){AND, ABSOLUTE_X_WITH_PENALTY};
  case 0x39:
    return (CPUInstruction){AND, ABSOLUTE_Y_WITH_PENALTY};
  case 0x21:
    return (CPUInstruction){AND, INDEXED_INDIRECT};
  case 0x31:
    return (CPUInstruction){AND, INDIRECT_INDEXED_WITH_PENALTY};

  case 0x0A:
    return (CPUInstruction){ASL, ACCUMULATOR};
  case 0x06:
    return (CPUInstruction){ASL, ZERO_PAGE};
  case 0x16:
    return (CPUInstruction){ASL, ZERO_PAGE_X};
  case 0x0E:
    return (CPUInstruction){ASL, ABSOLUTE};
  case 0x1E:
    return (CPUInstruction){ASL, ABSOLUTE_X};

  case 0x90:
    return (CPUInstruction){BCC, RELATIVE};
  case 0xB0:
    return (CPUInstruction){BCS, RELATIVE};
  case 0xF0:
    return (CPUInstruction){BEQ, RELATIVE};

  case 0x24:
    return (CPUInstruction){BIT, ZERO_PAGE};
  case 0x2C:
    return (CPUInstruction){BIT, ABSOLUTE};

  case 0x30:
    return (CPUInstruction){BMI, RELATIVE};
  case 0xD0:
    return (CPUInstruction){BNE, RELATIVE};
  case 0x10:
    return (CPUInstruction){BPL, RELATIVE};

  case 0x00:
    return (CPUInstruction){BRK, IMPLICIT};

  case 0x50:
    return (CPUInstruction){BVC, RELATIVE};
  case 0x70:
    return (CPUInstruction){BVS, RELATIVE};

  case 0x18:
    return (CPUInstruction){CLC, IMPLICIT};
  case 0xD8:
    return (CPUInstruction){CLD, IMPLICIT};
  case 0x58:
    return (CPUInstruction){CLI, IMPLICIT};
  case 0xB8:
    return (CPUInstruction){CLV, IMPLICIT};

  case 0xC9:
    return (CPUInstruction){CMP, IMMEDIATE};
  case 0xC5:
    return (CPUInstruction){CMP, ZERO_PAGE};
  case 0xD5:
    return (CPUInstruction){CMP, ZERO_PAGE_X};
  case 0xCD:
    return (CPUInstruction){CMP, ABSOLUTE};
  case 0xDD:
    return (CPUInstruction){CMP, ABSOLUTE_X_WITH_PENALTY};
  case 0xD9:
    return (CPUInstruction){CMP, ABSOLUTE_Y_WITH_PENALTY};
  case 0xC1:
    return (CPUInstruction){CMP, INDEXED_INDIRECT};
  case 0xD1:
    return (CPUInstruction){CMP, INDIRECT_INDEXED_WITH_PENALTY};

  case 0xE0:
    return (CPUInstruction){CPX, IMMEDIATE};
  case 0xE4:
    return (CPUInstruction){CPX, ZERO_PAGE};
  case 0xEC:
    return (CPUInstruction){CPX, ABSOLUTE};
  case 0xC0:
    return (CPUInstruction){CPY, IMMEDIATE};
  case 0xC4:
    return (CPUInstruction){CPY, ZERO_PAGE};
  case 0xCC:
    return (CPUInstruction){CPY, ABSOLUTE};

  case 0xC6:
    return (CPUInstruction){DEC, ZERO_PAGE};
  case 0xD6:
    return (CPUInstruction){DEC, ZERO_PAGE_X};
  case 0xCE:
    return (CPUInstruction){DEC, ABSOLUTE};
  case 0xDE:
    return (CPUInstruction){DEC, ABSOLUTE_X};

  case 0xCA:
    return (CPUInstruction){DEX, IMPLICIT};
  case 0x88:
    return (CPUInstruction){DEY, IMPLICIT};

  case 0x49:
    return (CPUInstruction){EOR, IMMEDIATE};
  case 0x45:
    return (CPUInstruction){EOR, ZERO_PAGE};
  case 0x55:
    return (CPUInstruction){EOR, ZERO_PAGE_X};
  case 0x4D:
    return (CPUInstruction){EOR, ABSOLUTE};
  case 0x5D:
    return (CPUInstruction){EOR, ABSOLUTE_X_WITH_PENALTY};
  case 0x59:
    return (CPUInstruction){EOR, ABSOLUTE_Y_WITH_PENALTY};
  case 0x41:
    return (CPUInstruction){EOR, INDEXED_INDIRECT};
  case 0x51:
    return (CPUInstruction){EOR, INDIRECT_INDEXED_WITH_PENALTY};

  case 0xE6:
    return (CPUInstruction){INC, ZERO_PAGE};
  case 0xF6:
    return (CPUInstruction){INC, ZERO_PAGE_X};
  case 0xEE:
    return (CPUInstruction){INC, ABSOLUTE};
  case 0xFE:
    return (CPUInstruction){INC, ABSOLUTE_X};

  case 0xE8:
    return (CPUInstruction){INX, IMPLICIT};
  case 0xC8:
    return (CPUInstruction){INY, IMPLICIT};

  case 0x4C:
    return (CPUInstruction){JMP, ABSOLUTE};
  case 0x6C:
    return (CPUInstruction){JMP, INDIRECT};

  case 0x20:
    return (CPUInstruction){JSR, ABSOLUTE};

  case 0xA9:
    return (CPUInstruction){LDA, IMMEDIATE};
  case 0xA5:
    return (CPUInstruction){LDA, ZERO_PAGE};
  case 0xB5:
    return (CPUInstruction){LDA, ZERO_PAGE_X};
  case 0xAD:
    return (CPUInstruction){LDA, ABSOLUTE};
  case 0xBD:
    return (CPUInstruction){LDA, ABSOLUTE_X_WITH_PENALTY};
  case 0xB9:
    return (CPUInstruction){LDA, ABSOLUTE_Y_WITH_PENALTY};
  case 0xA1:
    return (CPUInstruction){LDA, INDEXED_INDIRECT};
  case 0xB1:
    return (CPUInstruction){LDA, INDIRECT_INDEXED_WITH_PENALTY};

  case 0xA2:
    return (CPUInstruction){LDX, IMMEDIATE};
  case 0xA6:
    return (CPUInstruction){LDX, ZERO_PAGE};
  case 0xB6:
    return (CPUInstruction){LDX, ZERO_PAGE_Y};
  case 0xAE:
    return (CPUInstruction){LDX, ABSOLUTE};
  case 0xBE:
    return (CPUInstruction){LDX, ABSOLUTE_Y_WITH_PENALTY};

  case 0xA0:
    return (CPUInstruction){LDY, IMMEDIATE};
  case 0xA4:
    return (CPUInstruction){LDY, ZERO_PAGE};
  case 0xB4:
    return (CPUInstruction){LDY, ZERO_PAGE_X};
  case 0xAC:
    return (CPUInstruction){LDY, ABSOLUTE};
  case 0xBC:
    return (CPUInstruction){LDY, ABSOLUTE_X_WITH_PENALTY};

  case 0x4A:
    return (CPUInstruction){LSR, ACCUMULATOR};
  case 0x46:
    return (CPUInstruction){LSR, ZERO_PAGE};
  case 0x56:
    return (CPUInstruction){LSR, ZERO_PAGE_X};
  case 0x4E:
    return (CPUInstruction){LSR, ABSOLUTE};
  case 0x5E:
    return (CPUInstruction){LSR, ABSOLUTE_X};

  case 0x09:
    return (CPUInstruction){ORA, IMMEDIATE};
  case 0x05:
    return (CPUInstruction){ORA, ZERO_PAGE};
  case 0x15:
    return (CPUInstruction){ORA, ZERO_PAGE_X};
  case 0x0D:
    return (CPUInstruction){ORA, ABSOLUTE};
  case 0x1D:
    return (CPUInstruction){ORA, ABSOLUTE_X_WITH_PENALTY};
  case 0x19:
    return (CPUInstruction){ORA, ABSOLUTE_Y_WITH_PENALTY};
  case 0x01:
    return (CPUInstruction){ORA, INDEXED_INDIRECT};
  case 0x11:
    return (CPUInstruction){ORA, INDIRECT_INDEXED_WITH_PENALTY};

  case 0x48:
    return (CPUInstruction){PHA, IMPLICIT};
  case 0x08:
    return (CPUInstruction){PHP, IMPLICIT};
  case 0x68:
    return (CPUInstruction){PLA, IMPLICIT};
  case 0x28:
    return (CPUInstruction){PLP, IMPLICIT};

  case 0x2A:
    return (CPUInstruction){ROL, ACCUMULATOR};
  case 0x26:
    return (CPUInstruction){ROL, ZERO_PAGE};
  case 0x36:
    return (CPUInstruction){ROL, ZERO_PAGE_X};
  case 0x2E:
    return (CPUInstruction){ROL, ABSOLUTE};
  case 0x3E:
    return (CPUInstruction){ROL, ABSOLUTE_X};

  case 0x6A:
    return (CPUInstruction){ROR, ACCUMULATOR};
  case 0x66:
    return (CPUInstruction){ROR, ZERO_PAGE};
  case 0x76:
    return (CPUInstruction){ROR, ZERO_PAGE_X};
  case 0x6E:
    return (CPUInstruction){ROR, ABSOLUTE};
  case 0x7E:
    return (CPUInstruction){ROR, ABSOLUTE_X};

  case 0x40:
    return (CPUInstruction){RTI, IMPLICIT};
  case 0x60:
    return (CPUInstruction){RTS, IMPLICIT};

  case 0xE9:
    return (CPUInstruction){SBC, IMMEDIATE};
  case 0xE5:
    return (CPUInstruction){SBC, ZERO_PAGE};
  case 0xF5:
    return (CPUInstruction){SBC, ZERO_PAGE_X};
  case 0xED:
    return (CPUInstruction){SBC, ABSOLUTE};
  case 0xFD:
    return (CPUInstruction){SBC, ABSOLUTE_X_WITH_PENALTY};
  case 0xF9:
    return (CPUInstruction){SBC, ABSOLUTE_Y_WITH_PENALTY};
  case 0xE1:
    return (CPUInstruction){SBC, INDEXED_INDIRECT};
  case 0xF1:
    return (CPUInstruction){SBC, INDIRECT_INDEXED_WITH_PENALTY};

  case 0x38:
    return (CPUInstruction){SEC, IMPLICIT};
  case 0xF8:
    return (CPUInstruction){SED, IMPLICIT};
  case 0x78:
    return (CPUInstruction){SEI, IMPLICIT};

  case 0x85:
    return (CPUInstruction){STA, ZERO_PAGE};
  case 0x95:
    return (CPUInstruction){STA, ZERO_PAGE_X};
  case 0x8D:
    return (CPUInstruction){STA, ABSOLUTE};
  case 0x9D:
    return (CPUInstruction){STA, ABSOLUTE_X};
  case 0x99:
    return (CPUInstruction){STA, ABSOLUTE_Y};
  case 0x81:
    return (CPUInstruction){STA, INDEXED_INDIRECT};
  case 0x91:
    return (CPUInstruction){STA, INDIRECT_INDEXED};

  case 0x86:
    return (CPUInstruction){STX, ZERO_PAGE};
  case 0x96:
    return (CPUInstruction){STX, ZERO_PAGE_Y};
  case 0x8E:
    return (CPUInstruction){STX, ABSOLUTE};
  case 0x84:
    return (CPUInstruction){STY, ZERO_PAGE};
  case 0x94:
    return (CPUInstruction){STY, ZERO_PAGE_X};
  case 0x8C:
    return (CPUInstruction){STY, ABSOLUTE};

  case 0xAA:
    return (CPUInstruction){TAX, IMPLICIT};
  case 0xA8:
    return (CPUInstruction){TAY, IMPLICIT};
  case 0xBA:
    return (CPUInstruction){TSX, IMPLICIT};
  case 0x8A:
    return (CPUInstruction){TXA, IMPLICIT};
  case 0x9A:
    return (CPUInstruction){TXS, IMPLICIT};
  case 0x98:
    return (CPUInstruction){TYA, IMPLICIT};

    // clang-format off
  case 0x04: case 0x44: case 0x64:
    return (CPUInstruction){NOP, ZERO_PAGE};
  case 0x0C:
    return (CPUInstruction){NOP, ABSOLUTE};
  case 0x14: case 0x34: case 0x54: case 0x74: case 0xD4: case 0xF4:
    return (CPUInstruction){NOP, ZERO_PAGE_X};
  case 0x1A: case 0x3A: case 0x5A: case 0x7A: case 0xDA: case 0xEA: case 0xFA:
    return (CPUInstruction){NOP, IMPLICIT};
  case 0x1C: case 0x3C: case 0x5C: case 0x7C: case 0xDC: case 0xFC:
    return (CPUInstruction){NOP, ABSOLUTE_X_WITH_PENALTY};
  case 0x80: case 0x82: case 0x89: case 0xc2: case 0xE2:
    return (CPUInstruction){NOP, IMMEDIATE};
    // clang-format on

  // unofficial
  case 0xEB:
    return (CPUInstruction){SBC, IMMEDIATE};

  case 0xA3:
    return (CPUInstruction){LAX, INDEXED_INDIRECT};
  case 0xA7:
    return (CPUInstruction){LAX, ZERO_PAGE};
  case 0xAB:
    return (CPUInstruction){LAX, IMMEDIATE};
  case 0xAF:
    return (CPUInstruction){LAX, ABSOLUTE};
  case 0xB3:
    return (CPUInstruction){LAX, INDIRECT_INDEXED_WITH_PENALTY};
  case 0xB7:
    return (CPUInstruction){LAX, ZERO_PAGE_Y};
  case 0xBF:
    return (CPUInstruction){LAX, ABSOLUTE_Y_WITH_PENALTY};

  case 0x83:
    return (CPUInstruction){SAX, INDEXED_INDIRECT};
  case 0x87:
    return (CPUInstruction){SAX, ZERO_PAGE};
  case 0x8F:
    return (CPUInstruction){SAX, ABSOLUTE};
  case 0x97:
    return (CPUInstruction){SAX, ZERO_PAGE_Y};

  case 0xC3:
    return (CPUInstruction){DCP, INDEXED_INDIRECT};
  case 0xC7:
    return (CPUInstruction){DCP, ZERO_PAGE};
  case 0xCF:
    return (CPUInstruction){DCP, ABSOLUTE};
  case 0xD3:
    return (CPUInstruction){DCP, INDIRECT_INDEXED};
  case 0xD7:
    return (CPUInstruction){DCP, ZERO_PAGE_X};
  case 0xDB:
    return (CPUInstruction){DCP, ABSOLUTE_Y};
  case 0xDF:
    return (CPUInstruction){DCP, ABSOLUTE_X};

  case 0xE3:
    return (CPUInstruction){ISB, INDEXED_INDIRECT};
  case 0xE7:
    return (CPUInstruction){ISB, ZERO_PAGE};
  case 0xEF:
    return (CPUInstruction){ISB, ABSOLUTE};
  case 0xF3:
    return (CPUInstruction){ISB, INDIRECT_INDEXED};
  case 0xF7:
    return (CPUInstruction){ISB, ZERO_PAGE_X};
  case 0xFB:
    return (CPUInstruction){ISB, ABSOLUTE_Y};
  case 0xFF:
    return (CPUInstruction){ISB, ABSOLUTE_X};

  case 0x03:
    return (CPUInstruction){SLO, INDEXED_INDIRECT};
  case 0x07:
    return (CPUInstruction){SLO, ZERO_PAGE};
  case 0x0F:
    return (CPUInstruction){SLO, ABSOLUTE};
  case 0x13:
    return (CPUInstruction){SLO, INDIRECT_INDEXED};
  case 0x17:
    return (CPUInstruction){SLO, ZERO_PAGE_X};
  case 0x1B:
    return (CPUInstruction){SLO, ABSOLUTE_Y};
  case 0x1F:
    return (CPUInstruction){SLO, ABSOLUTE_X};

  case 0x23:
    return (CPUInstruction){RLA, INDEXED_INDIRECT};
  case 0x27:
    return (CPUInstruction){RLA, ZERO_PAGE};
  case 0x2F:
    return (CPUInstruction){RLA, ABSOLUTE};
  case 0x33:
    return (CPUInstruction){RLA, INDIRECT_INDEXED};
  case 0x37:
    return (CPUInstruction){RLA, ZERO_PAGE_X};
  case 0x3B:
    return (CPUInstruction){RLA, ABSOLUTE_Y};
  case 0x3F:
    return (CPUInstruction){RLA, ABSOLUTE_X};

  case 0x43:
    return (CPUInstruction){SRE, INDEXED_INDIRECT};
  case 0x47:
    return (CPUInstruction){SRE, ZERO_PAGE};
  case 0x4F:
    return (CPUInstruction){SRE, ABSOLUTE};
  case 0x53:
    return (CPUInstruction){SRE, INDIRECT_INDEXED};
  case 0x57:
    return (CPUInstruction){SRE, ZERO_PAGE_X};
  case 0x5B:
    return (CPUInstruction){SRE, ABSOLUTE_Y};
  case 0x5F:
    return (CPUInstruction){SRE, ABSOLUTE_X};

  case 0x63:
    return (CPUInstruction){RRA, INDEXED_INDIRECT};
  case 0x67:
    return (CPUInstruction){RRA, ZERO_PAGE};
  case 0x6F:
    return (CPUInstruction){RRA, ABSOLUTE};
  case 0x73:
    return (CPUInstruction){RRA, INDIRECT_INDEXED};
  case 0x77:
    return (CPUInstruction){RRA, ZERO_PAGE_X};
  case 0x7B:
    return (CPUInstruction){RRA, ABSOLUTE_Y};
  case 0x7F:
    return (CPUInstruction){RRA, ABSOLUTE_X};

  default:
    return (CPUInstruction){NOP, IMPLICIT};
  }
}
