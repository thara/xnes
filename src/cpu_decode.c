#include "cpu_decode.h"

cpu_instruction cpu_decode(uint8_t opcode) {
  switch (opcode) {
  case 0x69:
    return (cpu_instruction){ADC, IMMEDIATE};
  case 0x65:
    return (cpu_instruction){ADC, ZERO_PAGE};
  case 0x75:
    return (cpu_instruction){ADC, ZERO_PAGE_X};
  case 0x6D:
    return (cpu_instruction){ADC, ABSOLUTE};
  case 0x7D:
    return (cpu_instruction){ADC, ABSOLUTE_X_WITH_PENALTY};
  case 0x79:
    return (cpu_instruction){ADC, ABSOLUTE_Y_WITH_PENALTY};
  case 0x61:
    return (cpu_instruction){ADC, INDEXED_INDIRECT};
  case 0x71:
    return (cpu_instruction){ADC, INDIRECT_INDEXED_WITH_PENALTY};

  case 0x29:
    return (cpu_instruction){AND, IMMEDIATE};
  case 0x25:
    return (cpu_instruction){AND, ZERO_PAGE};
  case 0x35:
    return (cpu_instruction){AND, ZERO_PAGE_X};
  case 0x2D:
    return (cpu_instruction){AND, ABSOLUTE};
  case 0x3D:
    return (cpu_instruction){AND, ABSOLUTE_X_WITH_PENALTY};
  case 0x39:
    return (cpu_instruction){AND, ABSOLUTE_Y_WITH_PENALTY};
  case 0x21:
    return (cpu_instruction){AND, INDEXED_INDIRECT};
  case 0x31:
    return (cpu_instruction){AND, INDIRECT_INDEXED_WITH_PENALTY};

  case 0x0A:
    return (cpu_instruction){ASL, ACCUMULATOR};
  case 0x06:
    return (cpu_instruction){ASL, ZERO_PAGE};
  case 0x16:
    return (cpu_instruction){ASL, ZERO_PAGE_X};
  case 0x0E:
    return (cpu_instruction){ASL, ABSOLUTE};
  case 0x1E:
    return (cpu_instruction){ASL, ABSOLUTE_X};

  case 0x90:
    return (cpu_instruction){BCC, RELATIVE};
  case 0xB0:
    return (cpu_instruction){BCS, RELATIVE};
  case 0xF0:
    return (cpu_instruction){BEQ, RELATIVE};

  case 0x24:
    return (cpu_instruction){BIT, ZERO_PAGE};
  case 0x2C:
    return (cpu_instruction){BIT, ABSOLUTE};

  case 0x30:
    return (cpu_instruction){BMI, RELATIVE};
  case 0xD0:
    return (cpu_instruction){BNE, RELATIVE};
  case 0x10:
    return (cpu_instruction){BPL, RELATIVE};

  case 0x00:
    return (cpu_instruction){BRK, IMPLICIT};

  case 0x50:
    return (cpu_instruction){BVC, RELATIVE};
  case 0x70:
    return (cpu_instruction){BVS, RELATIVE};

  case 0x18:
    return (cpu_instruction){CLC, IMPLICIT};
  case 0xD8:
    return (cpu_instruction){CLD, IMPLICIT};
  case 0x58:
    return (cpu_instruction){CLI, IMPLICIT};
  case 0xB8:
    return (cpu_instruction){CLV, IMPLICIT};

  case 0xC9:
    return (cpu_instruction){CMP, IMMEDIATE};
  case 0xC5:
    return (cpu_instruction){CMP, ZERO_PAGE};
  case 0xD5:
    return (cpu_instruction){CMP, ZERO_PAGE_X};
  case 0xCD:
    return (cpu_instruction){CMP, ABSOLUTE};
  case 0xDD:
    return (cpu_instruction){CMP, ABSOLUTE_X_WITH_PENALTY};
  case 0xD9:
    return (cpu_instruction){CMP, ABSOLUTE_Y_WITH_PENALTY};
  case 0xC1:
    return (cpu_instruction){CMP, INDEXED_INDIRECT};
  case 0xD1:
    return (cpu_instruction){CMP, INDIRECT_INDEXED_WITH_PENALTY};

  case 0xE0:
    return (cpu_instruction){CPX, IMMEDIATE};
  case 0xE4:
    return (cpu_instruction){CPX, ZERO_PAGE};
  case 0xEC:
    return (cpu_instruction){CPX, ABSOLUTE};
  case 0xC0:
    return (cpu_instruction){CPY, IMMEDIATE};
  case 0xC4:
    return (cpu_instruction){CPY, ZERO_PAGE};
  case 0xCC:
    return (cpu_instruction){CPY, ABSOLUTE};

  case 0xC6:
    return (cpu_instruction){DEC, ZERO_PAGE};
  case 0xD6:
    return (cpu_instruction){DEC, ZERO_PAGE_X};
  case 0xCE:
    return (cpu_instruction){DEC, ABSOLUTE};
  case 0xDE:
    return (cpu_instruction){DEC, ABSOLUTE_X};

  case 0xCA:
    return (cpu_instruction){DEX, IMPLICIT};
  case 0x88:
    return (cpu_instruction){DEY, IMPLICIT};

  case 0x49:
    return (cpu_instruction){EOR, IMMEDIATE};
  case 0x45:
    return (cpu_instruction){EOR, ZERO_PAGE};
  case 0x55:
    return (cpu_instruction){EOR, ZERO_PAGE_X};
  case 0x4D:
    return (cpu_instruction){EOR, ABSOLUTE};
  case 0x5D:
    return (cpu_instruction){EOR, ABSOLUTE_X_WITH_PENALTY};
  case 0x59:
    return (cpu_instruction){EOR, ABSOLUTE_Y_WITH_PENALTY};
  case 0x41:
    return (cpu_instruction){EOR, INDEXED_INDIRECT};
  case 0x51:
    return (cpu_instruction){EOR, INDIRECT_INDEXED_WITH_PENALTY};

  case 0xE6:
    return (cpu_instruction){INC, ZERO_PAGE};
  case 0xF6:
    return (cpu_instruction){INC, ZERO_PAGE_X};
  case 0xEE:
    return (cpu_instruction){INC, ABSOLUTE};
  case 0xFE:
    return (cpu_instruction){INC, ABSOLUTE_X};

  case 0xE8:
    return (cpu_instruction){INX, IMPLICIT};
  case 0xC8:
    return (cpu_instruction){INY, IMPLICIT};

  case 0x4C:
    return (cpu_instruction){JMP, ABSOLUTE};
  case 0x6C:
    return (cpu_instruction){JMP, INDIRECT};

  case 0x20:
    return (cpu_instruction){JSR, ABSOLUTE};

  case 0xA9:
    return (cpu_instruction){LDA, IMMEDIATE};
  case 0xA5:
    return (cpu_instruction){LDA, ZERO_PAGE};
  case 0xB5:
    return (cpu_instruction){LDA, ZERO_PAGE_X};
  case 0xAD:
    return (cpu_instruction){LDA, ABSOLUTE};
  case 0xBD:
    return (cpu_instruction){LDA, ABSOLUTE_X_WITH_PENALTY};
  case 0xB9:
    return (cpu_instruction){LDA, ABSOLUTE_Y_WITH_PENALTY};
  case 0xA1:
    return (cpu_instruction){LDA, INDEXED_INDIRECT};
  case 0xB1:
    return (cpu_instruction){LDA, INDIRECT_INDEXED_WITH_PENALTY};

  case 0xA2:
    return (cpu_instruction){LDX, IMMEDIATE};
  case 0xA6:
    return (cpu_instruction){LDX, ZERO_PAGE};
  case 0xB6:
    return (cpu_instruction){LDX, ZERO_PAGE_Y};
  case 0xAE:
    return (cpu_instruction){LDX, ABSOLUTE};
  case 0xBE:
    return (cpu_instruction){LDX, ABSOLUTE_Y_WITH_PENALTY};

  case 0xA0:
    return (cpu_instruction){LDY, IMMEDIATE};
  case 0xA4:
    return (cpu_instruction){LDY, ZERO_PAGE};
  case 0xB4:
    return (cpu_instruction){LDY, ZERO_PAGE_X};
  case 0xAC:
    return (cpu_instruction){LDY, ABSOLUTE};
  case 0xBC:
    return (cpu_instruction){LDY, ABSOLUTE_X_WITH_PENALTY};

  case 0x4A:
    return (cpu_instruction){LSR, ACCUMULATOR};
  case 0x46:
    return (cpu_instruction){LSR, ZERO_PAGE};
  case 0x56:
    return (cpu_instruction){LSR, ZERO_PAGE_X};
  case 0x4E:
    return (cpu_instruction){LSR, ABSOLUTE};
  case 0x5E:
    return (cpu_instruction){LSR, ABSOLUTE_X};

  case 0x09:
    return (cpu_instruction){ORA, IMMEDIATE};
  case 0x05:
    return (cpu_instruction){ORA, ZERO_PAGE};
  case 0x15:
    return (cpu_instruction){ORA, ZERO_PAGE_X};
  case 0x0D:
    return (cpu_instruction){ORA, ABSOLUTE};
  case 0x1D:
    return (cpu_instruction){ORA, ABSOLUTE_X_WITH_PENALTY};
  case 0x19:
    return (cpu_instruction){ORA, ABSOLUTE_Y_WITH_PENALTY};
  case 0x01:
    return (cpu_instruction){ORA, INDEXED_INDIRECT};
  case 0x11:
    return (cpu_instruction){ORA, INDIRECT_INDEXED_WITH_PENALTY};

  case 0x48:
    return (cpu_instruction){PHA, IMPLICIT};
  case 0x08:
    return (cpu_instruction){PHP, IMPLICIT};
  case 0x68:
    return (cpu_instruction){PLA, IMPLICIT};
  case 0x28:
    return (cpu_instruction){PLP, IMPLICIT};

  case 0x2A:
    return (cpu_instruction){ROL, ACCUMULATOR};
  case 0x26:
    return (cpu_instruction){ROL, ZERO_PAGE};
  case 0x36:
    return (cpu_instruction){ROL, ZERO_PAGE_X};
  case 0x2E:
    return (cpu_instruction){ROL, ABSOLUTE};
  case 0x3E:
    return (cpu_instruction){ROL, ABSOLUTE_X};

  case 0x6A:
    return (cpu_instruction){ROR, ACCUMULATOR};
  case 0x66:
    return (cpu_instruction){ROR, ZERO_PAGE};
  case 0x76:
    return (cpu_instruction){ROR, ZERO_PAGE_X};
  case 0x6E:
    return (cpu_instruction){ROR, ABSOLUTE};
  case 0x7E:
    return (cpu_instruction){ROR, ABSOLUTE_X};

  case 0x40:
    return (cpu_instruction){RTI, IMPLICIT};
  case 0x60:
    return (cpu_instruction){RTS, IMPLICIT};

  case 0xE9:
    return (cpu_instruction){SBC, IMMEDIATE};
  case 0xE5:
    return (cpu_instruction){SBC, ZERO_PAGE};
  case 0xF5:
    return (cpu_instruction){SBC, ZERO_PAGE_X};
  case 0xED:
    return (cpu_instruction){SBC, ABSOLUTE};
  case 0xFD:
    return (cpu_instruction){SBC, ABSOLUTE_X_WITH_PENALTY};
  case 0xF9:
    return (cpu_instruction){SBC, ABSOLUTE_Y_WITH_PENALTY};
  case 0xE1:
    return (cpu_instruction){SBC, INDEXED_INDIRECT};
  case 0xF1:
    return (cpu_instruction){SBC, INDIRECT_INDEXED_WITH_PENALTY};

  case 0x38:
    return (cpu_instruction){SEC, IMPLICIT};
  case 0xF8:
    return (cpu_instruction){SED, IMPLICIT};
  case 0x78:
    return (cpu_instruction){SEI, IMPLICIT};

  case 0x85:
    return (cpu_instruction){STA, ZERO_PAGE};
  case 0x95:
    return (cpu_instruction){STA, ZERO_PAGE_X};
  case 0x8D:
    return (cpu_instruction){STA, ABSOLUTE};
  case 0x9D:
    return (cpu_instruction){STA, ABSOLUTE_X};
  case 0x99:
    return (cpu_instruction){STA, ABSOLUTE_Y};
  case 0x81:
    return (cpu_instruction){STA, INDEXED_INDIRECT};
  case 0x91:
    return (cpu_instruction){STA, INDIRECT_INDEXED};

  case 0x86:
    return (cpu_instruction){STX, ZERO_PAGE};
  case 0x96:
    return (cpu_instruction){STX, ZERO_PAGE_Y};
  case 0x8E:
    return (cpu_instruction){STX, ABSOLUTE};
  case 0x84:
    return (cpu_instruction){STY, ZERO_PAGE};
  case 0x94:
    return (cpu_instruction){STY, ZERO_PAGE_X};
  case 0x8C:
    return (cpu_instruction){STY, ABSOLUTE};

  case 0xAA:
    return (cpu_instruction){TAX, IMPLICIT};
  case 0xA8:
    return (cpu_instruction){TAY, IMPLICIT};
  case 0xBA:
    return (cpu_instruction){TSX, IMPLICIT};
  case 0x8A:
    return (cpu_instruction){TXA, IMPLICIT};
  case 0x9A:
    return (cpu_instruction){TXS, IMPLICIT};
  case 0x98:
    return (cpu_instruction){TYA, IMPLICIT};

    // clang-format off
  case 0x04: case 0x44: case 0x64:
    return (cpu_instruction){NOP, ZERO_PAGE};
  case 0x0C:
    return (cpu_instruction){NOP, ABSOLUTE};
  case 0x14: case 0x34: case 0x54: case 0x74: case 0xD4: case 0xF4:
    return (cpu_instruction){NOP, ZERO_PAGE_X};
  case 0x1A: case 0x3A: case 0x5A: case 0x7A: case 0xDA: case 0xEA: case 0xFA:
    return (cpu_instruction){NOP, IMPLICIT};
  case 0x1C: case 0x3C: case 0x5C: case 0x7C: case 0xDC: case 0xFC:
    return (cpu_instruction){NOP, ABSOLUTE_X_WITH_PENALTY};
  case 0x80: case 0x82: case 0x89: case 0xc2: case 0xE2:
    return (cpu_instruction){NOP, IMMEDIATE};
    // clang-format on

  // unofficial
  case 0xEB:
    return (cpu_instruction){SBC, IMMEDIATE};

  case 0xA3:
    return (cpu_instruction){LAX, INDEXED_INDIRECT};
  case 0xA7:
    return (cpu_instruction){LAX, ZERO_PAGE};
  case 0xAB:
    return (cpu_instruction){LAX, IMMEDIATE};
  case 0xAF:
    return (cpu_instruction){LAX, ABSOLUTE};
  case 0xB3:
    return (cpu_instruction){LAX, INDIRECT_INDEXED_WITH_PENALTY};
  case 0xB7:
    return (cpu_instruction){LAX, ZERO_PAGE_Y};
  case 0xBF:
    return (cpu_instruction){LAX, ABSOLUTE_Y_WITH_PENALTY};

  case 0x83:
    return (cpu_instruction){SAX, INDEXED_INDIRECT};
  case 0x87:
    return (cpu_instruction){SAX, ZERO_PAGE};
  case 0x8F:
    return (cpu_instruction){SAX, ABSOLUTE};
  case 0x97:
    return (cpu_instruction){SAX, ZERO_PAGE_Y};

  case 0xC3:
    return (cpu_instruction){DCP, INDEXED_INDIRECT};
  case 0xC7:
    return (cpu_instruction){DCP, ZERO_PAGE};
  case 0xCF:
    return (cpu_instruction){DCP, ABSOLUTE};
  case 0xD3:
    return (cpu_instruction){DCP, INDIRECT_INDEXED};
  case 0xD7:
    return (cpu_instruction){DCP, ZERO_PAGE_X};
  case 0xDB:
    return (cpu_instruction){DCP, ABSOLUTE_Y};
  case 0xDF:
    return (cpu_instruction){DCP, ABSOLUTE_X};

  case 0xE3:
    return (cpu_instruction){ISB, INDEXED_INDIRECT};
  case 0xE7:
    return (cpu_instruction){ISB, ZERO_PAGE};
  case 0xEF:
    return (cpu_instruction){ISB, ABSOLUTE};
  case 0xF3:
    return (cpu_instruction){ISB, INDIRECT_INDEXED};
  case 0xF7:
    return (cpu_instruction){ISB, ZERO_PAGE_X};
  case 0xFB:
    return (cpu_instruction){ISB, ABSOLUTE_Y};
  case 0xFF:
    return (cpu_instruction){ISB, ABSOLUTE_X};

  case 0x03:
    return (cpu_instruction){SLO, INDEXED_INDIRECT};
  case 0x07:
    return (cpu_instruction){SLO, ZERO_PAGE};
  case 0x0F:
    return (cpu_instruction){SLO, ABSOLUTE};
  case 0x13:
    return (cpu_instruction){SLO, INDIRECT_INDEXED};
  case 0x17:
    return (cpu_instruction){SLO, ZERO_PAGE_X};
  case 0x1B:
    return (cpu_instruction){SLO, ABSOLUTE_Y};
  case 0x1F:
    return (cpu_instruction){SLO, ABSOLUTE_X};

  case 0x23:
    return (cpu_instruction){RLA, INDEXED_INDIRECT};
  case 0x27:
    return (cpu_instruction){RLA, ZERO_PAGE};
  case 0x2F:
    return (cpu_instruction){RLA, ABSOLUTE};
  case 0x33:
    return (cpu_instruction){RLA, INDIRECT_INDEXED};
  case 0x37:
    return (cpu_instruction){RLA, ZERO_PAGE_X};
  case 0x3B:
    return (cpu_instruction){RLA, ABSOLUTE_Y};
  case 0x3F:
    return (cpu_instruction){RLA, ABSOLUTE_X};

  case 0x43:
    return (cpu_instruction){SRE, INDEXED_INDIRECT};
  case 0x47:
    return (cpu_instruction){SRE, ZERO_PAGE};
  case 0x4F:
    return (cpu_instruction){SRE, ABSOLUTE};
  case 0x53:
    return (cpu_instruction){SRE, INDIRECT_INDEXED};
  case 0x57:
    return (cpu_instruction){SRE, ZERO_PAGE_X};
  case 0x5B:
    return (cpu_instruction){SRE, ABSOLUTE_Y};
  case 0x5F:
    return (cpu_instruction){SRE, ABSOLUTE_X};

  case 0x63:
    return (cpu_instruction){RRA, INDEXED_INDIRECT};
  case 0x67:
    return (cpu_instruction){RRA, ZERO_PAGE};
  case 0x6F:
    return (cpu_instruction){RRA, ABSOLUTE};
  case 0x73:
    return (cpu_instruction){RRA, INDIRECT_INDEXED};
  case 0x77:
    return (cpu_instruction){RRA, ZERO_PAGE_X};
  case 0x7B:
    return (cpu_instruction){RRA, ABSOLUTE_Y};
  case 0x7F:
    return (cpu_instruction){RRA, ABSOLUTE_X};

  default:
    return (cpu_instruction){NOP, IMPLICIT};
  }
}
