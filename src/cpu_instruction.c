#include "cpu_instruction.h"

cpu_instruction cpu_decode(uint8_t opcode) {
  switch (opcode) {
  case 0x69:
    return (cpu_instruction){ADC, immediate};
  case 0x65:
    return (cpu_instruction){ADC, zero_page};
  case 0x75:
    return (cpu_instruction){ADC, zero_page_x};
  case 0x6D:
    return (cpu_instruction){ADC, absolute};
  case 0x7D:
    return (cpu_instruction){ADC, absolute_x_with_penalty};
  case 0x79:
    return (cpu_instruction){ADC, absolute_y_with_penalty};
  case 0x61:
    return (cpu_instruction){ADC, indexed_indirect};
  case 0x71:
    return (cpu_instruction){ADC, indirect_indexed_with_penalty};

  case 0x29:
    return (cpu_instruction){AND, immediate};
  case 0x25:
    return (cpu_instruction){AND, zero_page};
  case 0x35:
    return (cpu_instruction){AND, zero_page_x};
  case 0x2D:
    return (cpu_instruction){AND, absolute};
  case 0x3D:
    return (cpu_instruction){AND, absolute_x_with_penalty};
  case 0x39:
    return (cpu_instruction){AND, absolute_y_with_penalty};
  case 0x21:
    return (cpu_instruction){AND, indexed_indirect};
  case 0x31:
    return (cpu_instruction){AND, indirect_indexed_with_penalty};

  case 0x0A:
    return (cpu_instruction){ASL, accumulator};
  case 0x06:
    return (cpu_instruction){ASL, zero_page};
  case 0x16:
    return (cpu_instruction){ASL, zero_page_x};
  case 0x0E:
    return (cpu_instruction){ASL, absolute};
  case 0x1E:
    return (cpu_instruction){ASL, absolute_x};

  case 0x90:
    return (cpu_instruction){BCC, relative};
  case 0xB0:
    return (cpu_instruction){BCS, relative};
  case 0xF0:
    return (cpu_instruction){BEQ, relative};

  case 0x24:
    return (cpu_instruction){BIT, zero_page};
  case 0x2C:
    return (cpu_instruction){BIT, absolute};

  case 0x30:
    return (cpu_instruction){BMI, relative};
  case 0xD0:
    return (cpu_instruction){BNE, relative};
  case 0x10:
    return (cpu_instruction){BPL, relative};

  case 0x00:
    return (cpu_instruction){BRK, implicit};

  case 0x50:
    return (cpu_instruction){BVC, relative};
  case 0x70:
    return (cpu_instruction){BVS, relative};

  case 0x18:
    return (cpu_instruction){CLC, implicit};
  case 0xD8:
    return (cpu_instruction){CLD, implicit};
  case 0x58:
    return (cpu_instruction){CLI, implicit};
  case 0xB8:
    return (cpu_instruction){CLV, implicit};

  case 0xC9:
    return (cpu_instruction){CMP, immediate};
  case 0xC5:
    return (cpu_instruction){CMP, zero_page};
  case 0xD5:
    return (cpu_instruction){CMP, zero_page_x};
  case 0xCD:
    return (cpu_instruction){CMP, absolute};
  case 0xDD:
    return (cpu_instruction){CMP, absolute_x_with_penalty};
  case 0xD9:
    return (cpu_instruction){CMP, absolute_y_with_penalty};
  case 0xC1:
    return (cpu_instruction){CMP, indexed_indirect};
  case 0xD1:
    return (cpu_instruction){CMP, indirect_indexed_with_penalty};

  case 0xE0:
    return (cpu_instruction){CPX, immediate};
  case 0xE4:
    return (cpu_instruction){CPX, zero_page};
  case 0xEC:
    return (cpu_instruction){CPX, absolute};
  case 0xC0:
    return (cpu_instruction){CPY, immediate};
  case 0xC4:
    return (cpu_instruction){CPY, zero_page};
  case 0xCC:
    return (cpu_instruction){CPY, absolute};

  case 0xC6:
    return (cpu_instruction){DEC, zero_page};
  case 0xD6:
    return (cpu_instruction){DEC, zero_page_x};
  case 0xCE:
    return (cpu_instruction){DEC, absolute};
  case 0xDE:
    return (cpu_instruction){DEC, absolute_x};

  case 0xCA:
    return (cpu_instruction){DEX, implicit};
  case 0x88:
    return (cpu_instruction){DEY, implicit};

  case 0x49:
    return (cpu_instruction){EOR, immediate};
  case 0x45:
    return (cpu_instruction){EOR, zero_page};
  case 0x55:
    return (cpu_instruction){EOR, zero_page_x};
  case 0x4D:
    return (cpu_instruction){EOR, absolute};
  case 0x5D:
    return (cpu_instruction){EOR, absolute_x_with_penalty};
  case 0x59:
    return (cpu_instruction){EOR, absolute_y_with_penalty};
  case 0x41:
    return (cpu_instruction){EOR, indexed_indirect};
  case 0x51:
    return (cpu_instruction){EOR, indirect_indexed_with_penalty};

  case 0xE6:
    return (cpu_instruction){INC, zero_page};
  case 0xF6:
    return (cpu_instruction){INC, zero_page_x};
  case 0xEE:
    return (cpu_instruction){INC, absolute};
  case 0xFE:
    return (cpu_instruction){INC, absolute_x};

  case 0xE8:
    return (cpu_instruction){INX, implicit};
  case 0xC8:
    return (cpu_instruction){INY, implicit};

  case 0x4C:
    return (cpu_instruction){JMP, absolute};
  case 0x6C:
    return (cpu_instruction){JMP, indirect};

  case 0x20:
    return (cpu_instruction){JSR, absolute};

  case 0xA9:
    return (cpu_instruction){LDA, immediate};
  case 0xA5:
    return (cpu_instruction){LDA, zero_page};
  case 0xB5:
    return (cpu_instruction){LDA, zero_page_x};
  case 0xAD:
    return (cpu_instruction){LDA, absolute};
  case 0xBD:
    return (cpu_instruction){LDA, absolute_x_with_penalty};
  case 0xB9:
    return (cpu_instruction){LDA, absolute_y_with_penalty};
  case 0xA1:
    return (cpu_instruction){LDA, indexed_indirect};
  case 0xB1:
    return (cpu_instruction){LDA, indirect_indexed_with_penalty};

  case 0xA2:
    return (cpu_instruction){LDX, immediate};
  case 0xA6:
    return (cpu_instruction){LDX, zero_page};
  case 0xB6:
    return (cpu_instruction){LDX, zero_page_y};
  case 0xAE:
    return (cpu_instruction){LDX, absolute};
  case 0xBE:
    return (cpu_instruction){LDX, absolute_y_with_penalty};

  case 0xA0:
    return (cpu_instruction){LDY, immediate};
  case 0xA4:
    return (cpu_instruction){LDY, zero_page};
  case 0xB4:
    return (cpu_instruction){LDY, zero_page_x};
  case 0xAC:
    return (cpu_instruction){LDY, absolute};
  case 0xBC:
    return (cpu_instruction){LDY, absolute_x_with_penalty};

  case 0x4A:
    return (cpu_instruction){LSR, accumulator};
  case 0x46:
    return (cpu_instruction){LSR, zero_page};
  case 0x56:
    return (cpu_instruction){LSR, zero_page_x};
  case 0x4E:
    return (cpu_instruction){LSR, absolute};
  case 0x5E:
    return (cpu_instruction){LSR, absolute_x};

  case 0x09:
    return (cpu_instruction){ORA, immediate};
  case 0x05:
    return (cpu_instruction){ORA, zero_page};
  case 0x15:
    return (cpu_instruction){ORA, zero_page_x};
  case 0x0D:
    return (cpu_instruction){ORA, absolute};
  case 0x1D:
    return (cpu_instruction){ORA, absolute_x_with_penalty};
  case 0x19:
    return (cpu_instruction){ORA, absolute_y_with_penalty};
  case 0x01:
    return (cpu_instruction){ORA, indexed_indirect};
  case 0x11:
    return (cpu_instruction){ORA, indirect_indexed_with_penalty};

  case 0x48:
    return (cpu_instruction){PHA, implicit};
  case 0x08:
    return (cpu_instruction){PHP, implicit};
  case 0x68:
    return (cpu_instruction){PLA, implicit};
  case 0x28:
    return (cpu_instruction){PLP, implicit};

  case 0x2A:
    return (cpu_instruction){ROL, accumulator};
  case 0x26:
    return (cpu_instruction){ROL, zero_page};
  case 0x36:
    return (cpu_instruction){ROL, zero_page_x};
  case 0x2E:
    return (cpu_instruction){ROL, absolute};
  case 0x3E:
    return (cpu_instruction){ROL, absolute_x};

  case 0x6A:
    return (cpu_instruction){ROR, accumulator};
  case 0x66:
    return (cpu_instruction){ROR, zero_page};
  case 0x76:
    return (cpu_instruction){ROR, zero_page_x};
  case 0x6E:
    return (cpu_instruction){ROR, absolute};
  case 0x7E:
    return (cpu_instruction){ROR, absolute_x};

  case 0x40:
    return (cpu_instruction){RTI, implicit};
  case 0x60:
    return (cpu_instruction){RTS, implicit};

  case 0xE9:
    return (cpu_instruction){SBC, immediate};
  case 0xE5:
    return (cpu_instruction){SBC, zero_page};
  case 0xF5:
    return (cpu_instruction){SBC, zero_page_x};
  case 0xED:
    return (cpu_instruction){SBC, absolute};
  case 0xFD:
    return (cpu_instruction){SBC, absolute_x_with_penalty};
  case 0xF9:
    return (cpu_instruction){SBC, absolute_y_with_penalty};
  case 0xE1:
    return (cpu_instruction){SBC, indexed_indirect};
  case 0xF1:
    return (cpu_instruction){SBC, indirect_indexed_with_penalty};

  case 0x38:
    return (cpu_instruction){SEC, implicit};
  case 0xF8:
    return (cpu_instruction){SED, implicit};
  case 0x78:
    return (cpu_instruction){SEI, implicit};

  case 0x85:
    return (cpu_instruction){STA, zero_page};
  case 0x95:
    return (cpu_instruction){STA, zero_page_x};
  case 0x8D:
    return (cpu_instruction){STA, absolute};
  case 0x9D:
    return (cpu_instruction){STA, absolute_x};
  case 0x99:
    return (cpu_instruction){STA, absolute_y};
  case 0x81:
    return (cpu_instruction){STA, indexed_indirect};
  case 0x91:
    return (cpu_instruction){STA, indirect_indexed};

  case 0x86:
    return (cpu_instruction){STX, zero_page};
  case 0x96:
    return (cpu_instruction){STX, zero_page_y};
  case 0x8E:
    return (cpu_instruction){STX, absolute};
  case 0x84:
    return (cpu_instruction){STY, zero_page};
  case 0x94:
    return (cpu_instruction){STY, zero_page_x};
  case 0x8C:
    return (cpu_instruction){STY, absolute};

  case 0xAA:
    return (cpu_instruction){TAX, implicit};
  case 0xA8:
    return (cpu_instruction){TAY, implicit};
  case 0xBA:
    return (cpu_instruction){TSX, implicit};
  case 0x8A:
    return (cpu_instruction){TXA, implicit};
  case 0x9A:
    return (cpu_instruction){TXS, implicit};
  case 0x98:
    return (cpu_instruction){TYA, implicit};

    // clang-format off
  case 0x04: case 0x44: case 0x64:
    return (cpu_instruction){NOP, zero_page};
  case 0x0C:
    return (cpu_instruction){NOP, absolute};
  case 0x14: case 0x34: case 0x54: case 0x74: case 0xD4: case 0xF4:
    return (cpu_instruction){NOP, zero_page_x};
  case 0x1A: case 0x3A: case 0x5A: case 0x7A: case 0xDA: case 0xEA: case 0xFA:
    return (cpu_instruction){NOP, implicit};
  case 0x1C: case 0x3C: case 0x5C: case 0x7C: case 0xDC: case 0xFC:
    return (cpu_instruction){NOP, absolute_x_with_penalty};
  case 0x80: case 0x82: case 0x89: case 0xc2: case 0xE2:
    return (cpu_instruction){NOP, immediate};
    // clang-format on

  // unofficial
  case 0xEB:
    return (cpu_instruction){SBC, immediate};

  case 0xA3:
    return (cpu_instruction){LAX, indexed_indirect};
  case 0xA7:
    return (cpu_instruction){LAX, zero_page};
  case 0xAB:
    return (cpu_instruction){LAX, immediate};
  case 0xAF:
    return (cpu_instruction){LAX, absolute};
  case 0xB3:
    return (cpu_instruction){LAX, indirect_indexed_with_penalty};
  case 0xB7:
    return (cpu_instruction){LAX, zero_page_y};
  case 0xBF:
    return (cpu_instruction){LAX, absolute_y_with_penalty};

  case 0x83:
    return (cpu_instruction){SAX, indexed_indirect};
  case 0x87:
    return (cpu_instruction){SAX, zero_page};
  case 0x8F:
    return (cpu_instruction){SAX, absolute};
  case 0x97:
    return (cpu_instruction){SAX, zero_page_y};

  case 0xC3:
    return (cpu_instruction){DCP, indexed_indirect};
  case 0xC7:
    return (cpu_instruction){DCP, zero_page};
  case 0xCF:
    return (cpu_instruction){DCP, absolute};
  case 0xD3:
    return (cpu_instruction){DCP, indirect_indexed};
  case 0xD7:
    return (cpu_instruction){DCP, zero_page_x};
  case 0xDB:
    return (cpu_instruction){DCP, absolute_y};
  case 0xDF:
    return (cpu_instruction){DCP, absolute_x};

  case 0xE3:
    return (cpu_instruction){ISB, indexed_indirect};
  case 0xE7:
    return (cpu_instruction){ISB, zero_page};
  case 0xEF:
    return (cpu_instruction){ISB, absolute};
  case 0xF3:
    return (cpu_instruction){ISB, indirect_indexed};
  case 0xF7:
    return (cpu_instruction){ISB, zero_page_x};
  case 0xFB:
    return (cpu_instruction){ISB, absolute_y};
  case 0xFF:
    return (cpu_instruction){ISB, absolute_x};

  case 0x03:
    return (cpu_instruction){SLO, indexed_indirect};
  case 0x07:
    return (cpu_instruction){SLO, zero_page};
  case 0x0F:
    return (cpu_instruction){SLO, absolute};
  case 0x13:
    return (cpu_instruction){SLO, indirect_indexed};
  case 0x17:
    return (cpu_instruction){SLO, zero_page_x};
  case 0x1B:
    return (cpu_instruction){SLO, absolute_y};
  case 0x1F:
    return (cpu_instruction){SLO, absolute_x};

  case 0x23:
    return (cpu_instruction){RLA, indexed_indirect};
  case 0x27:
    return (cpu_instruction){RLA, zero_page};
  case 0x2F:
    return (cpu_instruction){RLA, absolute};
  case 0x33:
    return (cpu_instruction){RLA, indirect_indexed};
  case 0x37:
    return (cpu_instruction){RLA, zero_page_x};
  case 0x3B:
    return (cpu_instruction){RLA, absolute_y};
  case 0x3F:
    return (cpu_instruction){RLA, absolute_x};

  case 0x43:
    return (cpu_instruction){SRE, indexed_indirect};
  case 0x47:
    return (cpu_instruction){SRE, zero_page};
  case 0x4F:
    return (cpu_instruction){SRE, absolute};
  case 0x53:
    return (cpu_instruction){SRE, indirect_indexed};
  case 0x57:
    return (cpu_instruction){SRE, zero_page_x};
  case 0x5B:
    return (cpu_instruction){SRE, absolute_y};
  case 0x5F:
    return (cpu_instruction){SRE, absolute_x};

  case 0x63:
    return (cpu_instruction){RRA, indexed_indirect};
  case 0x67:
    return (cpu_instruction){RRA, zero_page};
  case 0x6F:
    return (cpu_instruction){RRA, absolute};
  case 0x73:
    return (cpu_instruction){RRA, indirect_indexed};
  case 0x77:
    return (cpu_instruction){RRA, zero_page_x};
  case 0x7B:
    return (cpu_instruction){RRA, absolute_y};
  case 0x7F:
    return (cpu_instruction){RRA, absolute_x};

  default:
    return (cpu_instruction){NOP, implicit};
  }
}
