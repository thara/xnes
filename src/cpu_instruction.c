#include "cpu_instruction.h"

CPUInstruction cpu_decode(uint8_t opcode) {
  switch (opcode) {
  case 0x69:
    return (CPUInstruction){ADC, immediate};
  case 0x65:
    return (CPUInstruction){ADC, zero_page};
  case 0x75:
    return (CPUInstruction){ADC, zero_page_x};
  case 0x6D:
    return (CPUInstruction){ADC, absolute};
  case 0x7D:
    return (CPUInstruction){ADC, absolute_x_with_penalty};
  case 0x79:
    return (CPUInstruction){ADC, absolute_y_with_penalty};
  case 0x61:
    return (CPUInstruction){ADC, indexed_indirect};
  case 0x71:
    return (CPUInstruction){ADC, indirect_indexed_with_penalty};

  case 0x29:
    return (CPUInstruction){AND, immediate};
  case 0x25:
    return (CPUInstruction){AND, zero_page};
  case 0x35:
    return (CPUInstruction){AND, zero_page_x};
  case 0x2D:
    return (CPUInstruction){AND, absolute};
  case 0x3D:
    return (CPUInstruction){AND, absolute_x_with_penalty};
  case 0x39:
    return (CPUInstruction){AND, absolute_y_with_penalty};
  case 0x21:
    return (CPUInstruction){AND, indexed_indirect};
  case 0x31:
    return (CPUInstruction){AND, indirect_indexed_with_penalty};

  case 0x0A:
    return (CPUInstruction){ASL, accumulator};
  case 0x06:
    return (CPUInstruction){ASL, zero_page};
  case 0x16:
    return (CPUInstruction){ASL, zero_page_x};
  case 0x0E:
    return (CPUInstruction){ASL, absolute};
  case 0x1E:
    return (CPUInstruction){ASL, absolute_x};

  case 0x90:
    return (CPUInstruction){BCC, relative};
  case 0xB0:
    return (CPUInstruction){BCS, relative};
  case 0xF0:
    return (CPUInstruction){BEQ, relative};

  case 0x24:
    return (CPUInstruction){BIT, zero_page};
  case 0x2C:
    return (CPUInstruction){BIT, absolute};

  case 0x30:
    return (CPUInstruction){BMI, relative};
  case 0xD0:
    return (CPUInstruction){BNE, relative};
  case 0x10:
    return (CPUInstruction){BPL, relative};

  case 0x00:
    return (CPUInstruction){BRK, implicit};

  case 0x50:
    return (CPUInstruction){BVC, relative};
  case 0x70:
    return (CPUInstruction){BVS, relative};

  case 0x18:
    return (CPUInstruction){CLC, implicit};
  case 0xD8:
    return (CPUInstruction){CLD, implicit};
  case 0x58:
    return (CPUInstruction){CLI, implicit};
  case 0xB8:
    return (CPUInstruction){CLV, implicit};

  case 0xC9:
    return (CPUInstruction){CMP, immediate};
  case 0xC5:
    return (CPUInstruction){CMP, zero_page};
  case 0xD5:
    return (CPUInstruction){CMP, zero_page_x};
  case 0xCD:
    return (CPUInstruction){CMP, absolute};
  case 0xDD:
    return (CPUInstruction){CMP, absolute_x_with_penalty};
  case 0xD9:
    return (CPUInstruction){CMP, absolute_y_with_penalty};
  case 0xC1:
    return (CPUInstruction){CMP, indexed_indirect};
  case 0xD1:
    return (CPUInstruction){CMP, indirect_indexed_with_penalty};

  case 0xE0:
    return (CPUInstruction){CPX, immediate};
  case 0xE4:
    return (CPUInstruction){CPX, zero_page};
  case 0xEC:
    return (CPUInstruction){CPX, absolute};
  case 0xC0:
    return (CPUInstruction){CPY, immediate};
  case 0xC4:
    return (CPUInstruction){CPY, zero_page};
  case 0xCC:
    return (CPUInstruction){CPY, absolute};

  case 0xC6:
    return (CPUInstruction){DEC, zero_page};
  case 0xD6:
    return (CPUInstruction){DEC, zero_page_x};
  case 0xCE:
    return (CPUInstruction){DEC, absolute};
  case 0xDE:
    return (CPUInstruction){DEC, absolute_x};

  case 0xCA:
    return (CPUInstruction){DEX, implicit};
  case 0x88:
    return (CPUInstruction){DEY, implicit};

  case 0x49:
    return (CPUInstruction){EOR, immediate};
  case 0x45:
    return (CPUInstruction){EOR, zero_page};
  case 0x55:
    return (CPUInstruction){EOR, zero_page_x};
  case 0x4D:
    return (CPUInstruction){EOR, absolute};
  case 0x5D:
    return (CPUInstruction){EOR, absolute_x_with_penalty};
  case 0x59:
    return (CPUInstruction){EOR, absolute_y_with_penalty};
  case 0x41:
    return (CPUInstruction){EOR, indexed_indirect};
  case 0x51:
    return (CPUInstruction){EOR, indirect_indexed_with_penalty};

  case 0xE6:
    return (CPUInstruction){INC, zero_page};
  case 0xF6:
    return (CPUInstruction){INC, zero_page_x};
  case 0xEE:
    return (CPUInstruction){INC, absolute};
  case 0xFE:
    return (CPUInstruction){INC, absolute_x};

  case 0xE8:
    return (CPUInstruction){INX, implicit};
  case 0xC8:
    return (CPUInstruction){INY, implicit};

  case 0x4C:
    return (CPUInstruction){JMP, absolute};
  case 0x6C:
    return (CPUInstruction){JMP, indirect};

  case 0x20:
    return (CPUInstruction){JSR, absolute};

  case 0xA9:
    return (CPUInstruction){LDA, immediate};
  case 0xA5:
    return (CPUInstruction){LDA, zero_page};
  case 0xB5:
    return (CPUInstruction){LDA, zero_page_x};
  case 0xAD:
    return (CPUInstruction){LDA, absolute};
  case 0xBD:
    return (CPUInstruction){LDA, absolute_x_with_penalty};
  case 0xB9:
    return (CPUInstruction){LDA, absolute_y_with_penalty};
  case 0xA1:
    return (CPUInstruction){LDA, indexed_indirect};
  case 0xB1:
    return (CPUInstruction){LDA, indirect_indexed_with_penalty};

  case 0xA2:
    return (CPUInstruction){LDX, immediate};
  case 0xA6:
    return (CPUInstruction){LDX, zero_page};
  case 0xB6:
    return (CPUInstruction){LDX, zero_page_y};
  case 0xAE:
    return (CPUInstruction){LDX, absolute};
  case 0xBE:
    return (CPUInstruction){LDX, absolute_y_with_penalty};

  case 0xA0:
    return (CPUInstruction){LDY, immediate};
  case 0xA4:
    return (CPUInstruction){LDY, zero_page};
  case 0xB4:
    return (CPUInstruction){LDY, zero_page_x};
  case 0xAC:
    return (CPUInstruction){LDY, absolute};
  case 0xBC:
    return (CPUInstruction){LDY, absolute_x_with_penalty};

  case 0x4A:
    return (CPUInstruction){LSR, accumulator};
  case 0x46:
    return (CPUInstruction){LSR, zero_page};
  case 0x56:
    return (CPUInstruction){LSR, zero_page_x};
  case 0x4E:
    return (CPUInstruction){LSR, absolute};
  case 0x5E:
    return (CPUInstruction){LSR, absolute_x};

  case 0x09:
    return (CPUInstruction){ORA, immediate};
  case 0x05:
    return (CPUInstruction){ORA, zero_page};
  case 0x15:
    return (CPUInstruction){ORA, zero_page_x};
  case 0x0D:
    return (CPUInstruction){ORA, absolute};
  case 0x1D:
    return (CPUInstruction){ORA, absolute_x_with_penalty};
  case 0x19:
    return (CPUInstruction){ORA, absolute_y_with_penalty};
  case 0x01:
    return (CPUInstruction){ORA, indexed_indirect};
  case 0x11:
    return (CPUInstruction){ORA, indirect_indexed_with_penalty};

  case 0x48:
    return (CPUInstruction){PHA, implicit};
  case 0x08:
    return (CPUInstruction){PHP, implicit};
  case 0x68:
    return (CPUInstruction){PLA, implicit};
  case 0x28:
    return (CPUInstruction){PLP, implicit};

  case 0x2A:
    return (CPUInstruction){ROL, accumulator};
  case 0x26:
    return (CPUInstruction){ROL, zero_page};
  case 0x36:
    return (CPUInstruction){ROL, zero_page_x};
  case 0x2E:
    return (CPUInstruction){ROL, absolute};
  case 0x3E:
    return (CPUInstruction){ROL, absolute_x};

  case 0x6A:
    return (CPUInstruction){ROR, accumulator};
  case 0x66:
    return (CPUInstruction){ROR, zero_page};
  case 0x76:
    return (CPUInstruction){ROR, zero_page_x};
  case 0x6E:
    return (CPUInstruction){ROR, absolute};
  case 0x7E:
    return (CPUInstruction){ROR, absolute_x};

  case 0x40:
    return (CPUInstruction){RTI, implicit};
  case 0x60:
    return (CPUInstruction){RTS, implicit};

  case 0xE9:
    return (CPUInstruction){SBC, immediate};
  case 0xE5:
    return (CPUInstruction){SBC, zero_page};
  case 0xF5:
    return (CPUInstruction){SBC, zero_page_x};
  case 0xED:
    return (CPUInstruction){SBC, absolute};
  case 0xFD:
    return (CPUInstruction){SBC, absolute_x_with_penalty};
  case 0xF9:
    return (CPUInstruction){SBC, absolute_y_with_penalty};
  case 0xE1:
    return (CPUInstruction){SBC, indexed_indirect};
  case 0xF1:
    return (CPUInstruction){SBC, indirect_indexed_with_penalty};

  case 0x38:
    return (CPUInstruction){SEC, implicit};
  case 0xF8:
    return (CPUInstruction){SED, implicit};
  case 0x78:
    return (CPUInstruction){SEI, implicit};

  case 0x85:
    return (CPUInstruction){STA, zero_page};
  case 0x95:
    return (CPUInstruction){STA, zero_page_x};
  case 0x8D:
    return (CPUInstruction){STA, absolute};
  case 0x9D:
    return (CPUInstruction){STA, absolute_x};
  case 0x99:
    return (CPUInstruction){STA, absolute_y};
  case 0x81:
    return (CPUInstruction){STA, indexed_indirect};
  case 0x91:
    return (CPUInstruction){STA, indirect_indexed};

  case 0x86:
    return (CPUInstruction){STX, zero_page};
  case 0x96:
    return (CPUInstruction){STX, zero_page_y};
  case 0x8E:
    return (CPUInstruction){STX, absolute};
  case 0x84:
    return (CPUInstruction){STY, zero_page};
  case 0x94:
    return (CPUInstruction){STY, zero_page_x};
  case 0x8C:
    return (CPUInstruction){STY, absolute};

  case 0xAA:
    return (CPUInstruction){TAX, implicit};
  case 0xA8:
    return (CPUInstruction){TAY, implicit};
  case 0xBA:
    return (CPUInstruction){TSX, implicit};
  case 0x8A:
    return (CPUInstruction){TXA, implicit};
  case 0x9A:
    return (CPUInstruction){TXS, implicit};
  case 0x98:
    return (CPUInstruction){TYA, implicit};

    // clang-format off
  case 0x04: case 0x44: case 0x64:
    return (CPUInstruction){NOP, zero_page};
  case 0x0C:
    return (CPUInstruction){NOP, absolute};
  case 0x14: case 0x34: case 0x54: case 0x74: case 0xD4: case 0xF4:
    return (CPUInstruction){NOP, zero_page_x};
  case 0x1A: case 0x3A: case 0x5A: case 0x7A: case 0xDA: case 0xEA: case 0xFA:
    return (CPUInstruction){NOP, implicit};
  case 0x1C: case 0x3C: case 0x5C: case 0x7C: case 0xDC: case 0xFC:
    return (CPUInstruction){NOP, absolute_x_with_penalty};
  case 0x80: case 0x82: case 0x89: case 0xc2: case 0xE2:
    return (CPUInstruction){NOP, immediate};
    // clang-format on

  // unofficial
  case 0xEB:
    return (CPUInstruction){SBC, immediate};

  case 0xA3:
    return (CPUInstruction){LAX, indexed_indirect};
  case 0xA7:
    return (CPUInstruction){LAX, zero_page};
  case 0xAB:
    return (CPUInstruction){LAX, immediate};
  case 0xAF:
    return (CPUInstruction){LAX, absolute};
  case 0xB3:
    return (CPUInstruction){LAX, indirect_indexed_with_penalty};
  case 0xB7:
    return (CPUInstruction){LAX, zero_page_y};
  case 0xBF:
    return (CPUInstruction){LAX, absolute_y_with_penalty};

  case 0x83:
    return (CPUInstruction){SAX, indexed_indirect};
  case 0x87:
    return (CPUInstruction){SAX, zero_page};
  case 0x8F:
    return (CPUInstruction){SAX, absolute};
  case 0x97:
    return (CPUInstruction){SAX, zero_page_y};

  case 0xC3:
    return (CPUInstruction){DCP, indexed_indirect};
  case 0xC7:
    return (CPUInstruction){DCP, zero_page};
  case 0xCF:
    return (CPUInstruction){DCP, absolute};
  case 0xD3:
    return (CPUInstruction){DCP, indirect_indexed};
  case 0xD7:
    return (CPUInstruction){DCP, zero_page_x};
  case 0xDB:
    return (CPUInstruction){DCP, absolute_y};
  case 0xDF:
    return (CPUInstruction){DCP, absolute_x};

  case 0xE3:
    return (CPUInstruction){ISB, indexed_indirect};
  case 0xE7:
    return (CPUInstruction){ISB, zero_page};
  case 0xEF:
    return (CPUInstruction){ISB, absolute};
  case 0xF3:
    return (CPUInstruction){ISB, indirect_indexed};
  case 0xF7:
    return (CPUInstruction){ISB, zero_page_x};
  case 0xFB:
    return (CPUInstruction){ISB, absolute_y};
  case 0xFF:
    return (CPUInstruction){ISB, absolute_x};

  case 0x03:
    return (CPUInstruction){SLO, indexed_indirect};
  case 0x07:
    return (CPUInstruction){SLO, zero_page};
  case 0x0F:
    return (CPUInstruction){SLO, absolute};
  case 0x13:
    return (CPUInstruction){SLO, indirect_indexed};
  case 0x17:
    return (CPUInstruction){SLO, zero_page_x};
  case 0x1B:
    return (CPUInstruction){SLO, absolute_y};
  case 0x1F:
    return (CPUInstruction){SLO, absolute_x};

  case 0x23:
    return (CPUInstruction){RLA, indexed_indirect};
  case 0x27:
    return (CPUInstruction){RLA, zero_page};
  case 0x2F:
    return (CPUInstruction){RLA, absolute};
  case 0x33:
    return (CPUInstruction){RLA, indirect_indexed};
  case 0x37:
    return (CPUInstruction){RLA, zero_page_x};
  case 0x3B:
    return (CPUInstruction){RLA, absolute_y};
  case 0x3F:
    return (CPUInstruction){RLA, absolute_x};

  case 0x43:
    return (CPUInstruction){SRE, indexed_indirect};
  case 0x47:
    return (CPUInstruction){SRE, zero_page};
  case 0x4F:
    return (CPUInstruction){SRE, absolute};
  case 0x53:
    return (CPUInstruction){SRE, indirect_indexed};
  case 0x57:
    return (CPUInstruction){SRE, zero_page_x};
  case 0x5B:
    return (CPUInstruction){SRE, absolute_y};
  case 0x5F:
    return (CPUInstruction){SRE, absolute_x};

  case 0x63:
    return (CPUInstruction){RRA, indexed_indirect};
  case 0x67:
    return (CPUInstruction){RRA, zero_page};
  case 0x6F:
    return (CPUInstruction){RRA, absolute};
  case 0x73:
    return (CPUInstruction){RRA, indirect_indexed};
  case 0x77:
    return (CPUInstruction){RRA, zero_page_x};
  case 0x7B:
    return (CPUInstruction){RRA, absolute_y};
  case 0x7F:
    return (CPUInstruction){RRA, absolute_x};

  default:
    return (CPUInstruction){NOP, implicit};
  }
}
