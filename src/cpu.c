#include <stdint.h>
#include <stdbool.h>

#include "nes.h"
#include "mem.h"

struct cpu {
  uint8_t A, X, Y;
  uint8_t S;
  uint8_t P;
  uint16_t PC;

  uintmax_t cycles;
};

void cpu_tick(struct nes *nes);
uint8_t cpu_read(struct nes *nes, uint16_t addr);
void cpu_write(struct nes *nes, uint16_t addr, uint8_t val);

void cpu_init(struct nes *nes) {
  // https://wiki.nesdev.com/w/index.php/CPU_power_up_state
  nes->cpu->A = 0x00;
  nes->cpu->X = 0x00;
  nes->cpu->Y = 0x00;
  nes->cpu->S = 0xFD;
  nes->cpu->P = 0x34;       // IRQ disabled
  cpu_write(nes, 0x4017, 0x00); // frame irq disabled
  cpu_write(nes, 0x4015, 0x00); // all channels disabled
}

// clang-format off
enum addressing_mode {
  implicit, accumulator, immediate,
  zero_page, zero_page_x, zero_page_y,
  absolute, absolute_x, absolute_x_with_penalty, absolute_y, absolute_y_with_penalty,
  relative,
  indirect, indexed_indirect, indirect_indexed, indirect_indexed_with_penalty,
};

enum mnemonic {
  // Load/Store Operations,
  LDA, LDX, LDY, STA, STX, STY,
  // Register Operations,
  TAX, TSX, TAY, TXA, TXS, TYA,
  // Stack instructions,
  PHA, PHP, PLA, PLP,
  // Logical instructions,
  AND, EOR, ORA, BIT,
  // Arithmetic instructions,
  ADC, SBC, CMP, CPX, CPY,
  // Increment/Decrement instructions,
  INC, INX, INY, DEC, DEX, DEY,
  // Shift instructions,
  ASL, LSR, ROL, ROR,
  // Jump instructions,
  JMP, JSR, RTS, RTI,
  // Branch instructions,
  BCC, BCS, BEQ, BMI, BNE, BPL, BVC, BVS,
  // Flag control instructions,
  CLC, CLD, CLI, CLV, SEC, SED, SEI,
  // Misc,
  BRK, NOP,
  // Unofficial,
  LAX, SAX, DCP, ISB, SLO, RLA, SRE, RRA,
};

// clang-format on

struct instruction {
  enum mnemonic mnemonic;
  enum addressing_mode mode;
};

struct instruction decode(uint8_t opcode);
void execute(struct nes *nes, struct instruction inst);

void cpu_step(struct nes *nes) {
  uint8_t op = cpu_read(nes, nes->cpu->PC);
  nes->cpu->PC++;

  struct instruction inst = decode(op);
  execute(nes, inst);
}

struct instruction decode(uint8_t opcode) {
  switch (opcode) {
  case 0x69:
    return (struct instruction){ADC, immediate};
  case 0x65:
    return (struct instruction){ADC, zero_page};
  case 0x75:
    return (struct instruction){ADC, zero_page_x};
  case 0x6D:
    return (struct instruction){ADC, absolute};
  case 0x7D:
    return (struct instruction){ADC, absolute_x_with_penalty};
  case 0x79:
    return (struct instruction){ADC, absolute_y_with_penalty};
  case 0x61:
    return (struct instruction){ADC, indexed_indirect};
  case 0x71:
    return (struct instruction){ADC, indirect_indexed_with_penalty};

  case 0x29:
    return (struct instruction){AND, immediate};
  case 0x25:
    return (struct instruction){AND, zero_page};
  case 0x35:
    return (struct instruction){AND, zero_page_x};
  case 0x2D:
    return (struct instruction){AND, absolute};
  case 0x3D:
    return (struct instruction){AND, absolute_x_with_penalty};
  case 0x39:
    return (struct instruction){AND, absolute_y_with_penalty};
  case 0x21:
    return (struct instruction){AND, indexed_indirect};
  case 0x31:
    return (struct instruction){AND, indirect_indexed_with_penalty};

  case 0x0A:
    return (struct instruction){ASL, accumulator};
  case 0x06:
    return (struct instruction){ASL, zero_page};
  case 0x16:
    return (struct instruction){ASL, zero_page_x};
  case 0x0E:
    return (struct instruction){ASL, absolute};
  case 0x1E:
    return (struct instruction){ASL, absolute_x};

  case 0x90:
    return (struct instruction){BCC, relative};
  case 0xB0:
    return (struct instruction){BCS, relative};
  case 0xF0:
    return (struct instruction){BEQ, relative};

  case 0x24:
    return (struct instruction){BIT, zero_page};
  case 0x2C:
    return (struct instruction){BIT, absolute};

  case 0x30:
    return (struct instruction){BMI, relative};
  case 0xD0:
    return (struct instruction){BNE, relative};
  case 0x10:
    return (struct instruction){BPL, relative};

  case 0x00:
    return (struct instruction){BRK, implicit};

  case 0x50:
    return (struct instruction){BVC, relative};
  case 0x70:
    return (struct instruction){BVS, relative};

  case 0x18:
    return (struct instruction){CLC, implicit};
  case 0xD8:
    return (struct instruction){CLD, implicit};
  case 0x58:
    return (struct instruction){CLI, implicit};
  case 0xB8:
    return (struct instruction){CLV, implicit};

  case 0xC9:
    return (struct instruction){CMP, immediate};
  case 0xC5:
    return (struct instruction){CMP, zero_page};
  case 0xD5:
    return (struct instruction){CMP, zero_page_x};
  case 0xCD:
    return (struct instruction){CMP, absolute};
  case 0xDD:
    return (struct instruction){CMP, absolute_x_with_penalty};
  case 0xD9:
    return (struct instruction){CMP, absolute_y_with_penalty};
  case 0xC1:
    return (struct instruction){CMP, indexed_indirect};
  case 0xD1:
    return (struct instruction){CMP, indirect_indexed_with_penalty};

  case 0xE0:
    return (struct instruction){CPX, immediate};
  case 0xE4:
    return (struct instruction){CPX, zero_page};
  case 0xEC:
    return (struct instruction){CPX, absolute};
  case 0xC0:
    return (struct instruction){CPY, immediate};
  case 0xC4:
    return (struct instruction){CPY, zero_page};
  case 0xCC:
    return (struct instruction){CPY, absolute};

  case 0xC6:
    return (struct instruction){DEC, zero_page};
  case 0xD6:
    return (struct instruction){DEC, zero_page_x};
  case 0xCE:
    return (struct instruction){DEC, absolute};
  case 0xDE:
    return (struct instruction){DEC, absolute_x};

  case 0xCA:
    return (struct instruction){DEX, implicit};
  case 0x88:
    return (struct instruction){DEY, implicit};

  case 0x49:
    return (struct instruction){EOR, immediate};
  case 0x45:
    return (struct instruction){EOR, zero_page};
  case 0x55:
    return (struct instruction){EOR, zero_page_x};
  case 0x4D:
    return (struct instruction){EOR, absolute};
  case 0x5D:
    return (struct instruction){EOR, absolute_x_with_penalty};
  case 0x59:
    return (struct instruction){EOR, absolute_y_with_penalty};
  case 0x41:
    return (struct instruction){EOR, indexed_indirect};
  case 0x51:
    return (struct instruction){EOR, indirect_indexed_with_penalty};

  case 0xE6:
    return (struct instruction){INC, zero_page};
  case 0xF6:
    return (struct instruction){INC, zero_page_x};
  case 0xEE:
    return (struct instruction){INC, absolute};
  case 0xFE:
    return (struct instruction){INC, absolute_x};

  case 0xE8:
    return (struct instruction){INX, implicit};
  case 0xC8:
    return (struct instruction){INY, implicit};

  case 0x4C:
    return (struct instruction){JMP, absolute};
  case 0x6C:
    return (struct instruction){JMP, indirect};

  case 0x20:
    return (struct instruction){JSR, absolute};

  case 0xA9:
    return (struct instruction){LDA, immediate};
  case 0xA5:
    return (struct instruction){LDA, zero_page};
  case 0xB5:
    return (struct instruction){LDA, zero_page_x};
  case 0xAD:
    return (struct instruction){LDA, absolute};
  case 0xBD:
    return (struct instruction){LDA, absolute_x_with_penalty};
  case 0xB9:
    return (struct instruction){LDA, absolute_y_with_penalty};
  case 0xA1:
    return (struct instruction){LDA, indexed_indirect};
  case 0xB1:
    return (struct instruction){LDA, indirect_indexed_with_penalty};

  case 0xA2:
    return (struct instruction){LDX, immediate};
  case 0xA6:
    return (struct instruction){LDX, zero_page};
  case 0xB6:
    return (struct instruction){LDX, zero_page_y};
  case 0xAE:
    return (struct instruction){LDX, absolute};
  case 0xBE:
    return (struct instruction){LDX, absolute_y_with_penalty};

  case 0xA0:
    return (struct instruction){LDY, immediate};
  case 0xA4:
    return (struct instruction){LDY, zero_page};
  case 0xB4:
    return (struct instruction){LDY, zero_page_x};
  case 0xAC:
    return (struct instruction){LDY, absolute};
  case 0xBC:
    return (struct instruction){LDY, absolute_x_with_penalty};

  case 0x4A:
    return (struct instruction){LSR, accumulator};
  case 0x46:
    return (struct instruction){LSR, zero_page};
  case 0x56:
    return (struct instruction){LSR, zero_page_x};
  case 0x4E:
    return (struct instruction){LSR, absolute};
  case 0x5E:
    return (struct instruction){LSR, absolute_x};

  case 0x09:
    return (struct instruction){ORA, immediate};
  case 0x05:
    return (struct instruction){ORA, zero_page};
  case 0x15:
    return (struct instruction){ORA, zero_page_x};
  case 0x0D:
    return (struct instruction){ORA, absolute};
  case 0x1D:
    return (struct instruction){ORA, absolute_x_with_penalty};
  case 0x19:
    return (struct instruction){ORA, absolute_y_with_penalty};
  case 0x01:
    return (struct instruction){ORA, indexed_indirect};
  case 0x11:
    return (struct instruction){ORA, indirect_indexed_with_penalty};

  case 0x48:
    return (struct instruction){PHA, implicit};
  case 0x08:
    return (struct instruction){PHP, implicit};
  case 0x68:
    return (struct instruction){PLA, implicit};
  case 0x28:
    return (struct instruction){PLP, implicit};

  case 0x2A:
    return (struct instruction){ROL, accumulator};
  case 0x26:
    return (struct instruction){ROL, zero_page};
  case 0x36:
    return (struct instruction){ROL, zero_page_x};
  case 0x2E:
    return (struct instruction){ROL, absolute};
  case 0x3E:
    return (struct instruction){ROL, absolute_x};

  case 0x6A:
    return (struct instruction){ROR, accumulator};
  case 0x66:
    return (struct instruction){ROR, zero_page};
  case 0x76:
    return (struct instruction){ROR, zero_page_x};
  case 0x6E:
    return (struct instruction){ROR, absolute};
  case 0x7E:
    return (struct instruction){ROR, absolute_x};

  case 0x40:
    return (struct instruction){RTI, implicit};
  case 0x60:
    return (struct instruction){RTS, implicit};

  case 0xE9:
    return (struct instruction){SBC, immediate};
  case 0xE5:
    return (struct instruction){SBC, zero_page};
  case 0xF5:
    return (struct instruction){SBC, zero_page_x};
  case 0xED:
    return (struct instruction){SBC, absolute};
  case 0xFD:
    return (struct instruction){SBC, absolute_x_with_penalty};
  case 0xF9:
    return (struct instruction){SBC, absolute_y_with_penalty};
  case 0xE1:
    return (struct instruction){SBC, indexed_indirect};
  case 0xF1:
    return (struct instruction){SBC, indirect_indexed_with_penalty};

  case 0x38:
    return (struct instruction){SEC, implicit};
  case 0xF8:
    return (struct instruction){SED, implicit};
  case 0x78:
    return (struct instruction){SEI, implicit};

  case 0x85:
    return (struct instruction){STA, zero_page};
  case 0x95:
    return (struct instruction){STA, zero_page_x};
  case 0x8D:
    return (struct instruction){STA, absolute};
  case 0x9D:
    return (struct instruction){STA, absolute_x};
  case 0x99:
    return (struct instruction){STA, absolute_y};
  case 0x81:
    return (struct instruction){STA, indexed_indirect};
  case 0x91:
    return (struct instruction){STA, indirect_indexed};

  case 0x86:
    return (struct instruction){STX, zero_page};
  case 0x96:
    return (struct instruction){STX, zero_page_y};
  case 0x8E:
    return (struct instruction){STX, absolute};
  case 0x84:
    return (struct instruction){STY, zero_page};
  case 0x94:
    return (struct instruction){STY, zero_page_x};
  case 0x8C:
    return (struct instruction){STY, absolute};

  case 0xAA:
    return (struct instruction){TAX, implicit};
  case 0xA8:
    return (struct instruction){TAY, implicit};
  case 0xBA:
    return (struct instruction){TSX, implicit};
  case 0x8A:
    return (struct instruction){TXA, implicit};
  case 0x9A:
    return (struct instruction){TXS, implicit};
  case 0x98:
    return (struct instruction){TYA, implicit};

    // clang-format off
  case 0x04: case 0x44: case 0x64:
    return (struct instruction){NOP, zero_page};
  case 0x0C:
    return (struct instruction){NOP, absolute};
  case 0x14: case 0x34: case 0x54: case 0x74: case 0xD4: case 0xF4:
    return (struct instruction){NOP, zero_page_x};
  case 0x1A: case 0x3A: case 0x5A: case 0x7A: case 0xDA: case 0xEA: case 0xFA:
    return (struct instruction){NOP, implicit};
  case 0x1C: case 0x3C: case 0x5C: case 0x7C: case 0xDC: case 0xFC:
    return (struct instruction){NOP, absolute_x_with_penalty};
  case 0x80: case 0x82: case 0x89: case 0xc2: case 0xE2:
    return (struct instruction){NOP, immediate};
    // clang-format on

  // unofficial
  case 0xEB:
    return (struct instruction){SBC, immediate};

  case 0xA3:
    return (struct instruction){LAX, indexed_indirect};
  case 0xA7:
    return (struct instruction){LAX, zero_page};
  case 0xAB:
    return (struct instruction){LAX, immediate};
  case 0xAF:
    return (struct instruction){LAX, absolute};
  case 0xB3:
    return (struct instruction){LAX, indirect_indexed_with_penalty};
  case 0xB7:
    return (struct instruction){LAX, zero_page_y};
  case 0xBF:
    return (struct instruction){LAX, absolute_y_with_penalty};

  case 0x83:
    return (struct instruction){SAX, indexed_indirect};
  case 0x87:
    return (struct instruction){SAX, zero_page};
  case 0x8F:
    return (struct instruction){SAX, absolute};
  case 0x97:
    return (struct instruction){SAX, zero_page_y};

  case 0xC3:
    return (struct instruction){DCP, indexed_indirect};
  case 0xC7:
    return (struct instruction){DCP, zero_page};
  case 0xCF:
    return (struct instruction){DCP, absolute};
  case 0xD3:
    return (struct instruction){DCP, indirect_indexed};
  case 0xD7:
    return (struct instruction){DCP, zero_page_x};
  case 0xDB:
    return (struct instruction){DCP, absolute_y};
  case 0xDF:
    return (struct instruction){DCP, absolute_x};

  case 0xE3:
    return (struct instruction){ISB, indexed_indirect};
  case 0xE7:
    return (struct instruction){ISB, zero_page};
  case 0xEF:
    return (struct instruction){ISB, absolute};
  case 0xF3:
    return (struct instruction){ISB, indirect_indexed};
  case 0xF7:
    return (struct instruction){ISB, zero_page_x};
  case 0xFB:
    return (struct instruction){ISB, absolute_y};
  case 0xFF:
    return (struct instruction){ISB, absolute_x};

  case 0x03:
    return (struct instruction){SLO, indexed_indirect};
  case 0x07:
    return (struct instruction){SLO, zero_page};
  case 0x0F:
    return (struct instruction){SLO, absolute};
  case 0x13:
    return (struct instruction){SLO, indirect_indexed};
  case 0x17:
    return (struct instruction){SLO, zero_page_x};
  case 0x1B:
    return (struct instruction){SLO, absolute_y};
  case 0x1F:
    return (struct instruction){SLO, absolute_x};

  case 0x23:
    return (struct instruction){RLA, indexed_indirect};
  case 0x27:
    return (struct instruction){RLA, zero_page};
  case 0x2F:
    return (struct instruction){RLA, absolute};
  case 0x33:
    return (struct instruction){RLA, indirect_indexed};
  case 0x37:
    return (struct instruction){RLA, zero_page_x};
  case 0x3B:
    return (struct instruction){RLA, absolute_y};
  case 0x3F:
    return (struct instruction){RLA, absolute_x};

  case 0x43:
    return (struct instruction){SRE, indexed_indirect};
  case 0x47:
    return (struct instruction){SRE, zero_page};
  case 0x4F:
    return (struct instruction){SRE, absolute};
  case 0x53:
    return (struct instruction){SRE, indirect_indexed};
  case 0x57:
    return (struct instruction){SRE, zero_page_x};
  case 0x5B:
    return (struct instruction){SRE, absolute_y};
  case 0x5F:
    return (struct instruction){SRE, absolute_x};

  case 0x63:
    return (struct instruction){RRA, indexed_indirect};
  case 0x67:
    return (struct instruction){RRA, zero_page};
  case 0x6F:
    return (struct instruction){RRA, absolute};
  case 0x73:
    return (struct instruction){RRA, indirect_indexed};
  case 0x77:
    return (struct instruction){RRA, zero_page_x};
  case 0x7B:
    return (struct instruction){RRA, absolute_y};
  case 0x7F:
    return (struct instruction){RRA, absolute_x};

  default:
    return (struct instruction){NOP, implicit};
  }
}

void cpu_tick(struct nes *nes) {
    nes_tick(nes);
    nes->cpu->cycles++;
}

uint8_t cpu_read(struct nes *nes, uint16_t addr) {
    uint8_t m = mem_read(nes, addr);
    cpu_tick(nes);
    return m;
}

void cpu_write(struct nes *nes, uint16_t addr, uint8_t val) {
    mem_write(nes, addr, val);
    cpu_tick(nes);
}

uint16_t cpu_read_word(struct nes *nes, uint16_t addr) {
  return cpu_read(nes, addr) | cpu_read(nes, addr + 1) << 8;
}

uint16_t read_on_indirect(struct nes *nes, uint16_t addr) {
  uint16_t low = cpu_read(nes, addr);
  // Reproduce 6502 bug - http://nesdev.com/6502bugs.txt
  uint16_t high = cpu_read(nes, (addr & 0xFF00) | ((addr + 1) & 0x00FF));
  return low | (high << 8);
}

bool is_page_crossed(uint16_t a, uint16_t b) {
  return ((a + b) & 0xFF00) != (b & 0xFF00);
}

uint16_t cpu_get_operand(struct nes *nes, enum addressing_mode mode) {
  uint16_t operand;

  switch (mode) {
  case implicit:
    operand = 0;
  case accumulator:
    operand = nes->cpu->A;
  case immediate:
    operand = ++nes->cpu->PC;
  case zero_page:
    operand = cpu_read(nes, nes->cpu->PC);
    nes->cpu->PC++;
  case zero_page_x:
    cpu_tick(nes);
    operand = (cpu_read(nes, nes->cpu->PC) + nes->cpu->X) & 0xFF;
    nes->cpu->PC++;
  case zero_page_y:
    cpu_tick(nes);
    operand = (cpu_read(nes, nes->cpu->PC) + nes->cpu->Y) & 0xFF;
    nes->cpu->PC++;
  case absolute:
    operand = cpu_read_word(nes, nes->cpu->PC);
    nes->cpu->PC += 2;
  case absolute_x:
    operand = cpu_read_word(nes, nes->cpu->PC);
    nes->cpu->PC += 2;
    cpu_tick(nes);
    operand += nes->cpu->X;
  case absolute_x_with_penalty:
    operand = cpu_read_word(nes, nes->cpu->PC);
    nes->cpu->PC += 2;
    if (is_page_crossed(nes->cpu->X, operand)) {
      cpu_tick(nes);
    }
    operand += nes->cpu->X;
  case absolute_y:
    operand = cpu_read_word(nes, nes->cpu->PC);
    nes->cpu->PC += 2;
    cpu_tick(nes);
    operand += nes->cpu->Y;
  case absolute_y_with_penalty:
    operand = cpu_read_word(nes, nes->cpu->PC);
    nes->cpu->PC += 2;
    if (is_page_crossed(nes->cpu->Y, operand)) {
      cpu_tick(nes);
    }
    operand += nes->cpu->Y;
  case relative:
    operand = cpu_read(nes, nes->cpu->PC);
    nes->cpu->PC++;
  case indirect:
    operand = read_on_indirect(nes, cpu_read_word(nes, nes->cpu->PC));
    nes->cpu->PC += 2;
  case indexed_indirect:
    operand = read_on_indirect(nes, cpu_read(nes, nes->cpu->PC) + nes->cpu->X);
    nes->cpu->PC += 2;
    cpu_tick(nes);
  case indirect_indexed:
    operand = read_on_indirect(nes, cpu_read(nes, nes->cpu->PC));
    nes->cpu->PC++;
    cpu_tick(nes);
    operand += nes->cpu->Y;
  case indirect_indexed_with_penalty:
    operand = read_on_indirect(nes, cpu_read(nes, nes->cpu->PC));
    nes->cpu->PC++;
    if (is_page_crossed(nes->cpu->Y, operand)) {
      cpu_tick(nes);
    }
    operand += nes->cpu->Y;
    // default:
    //   exit failure
  }
  return operand;
}

enum cpu_status {
  C, // Carry
  Z, // Zero
  I, // Interrupt Disable
  D, // Decimal
  V, // Overflow
  N, // Negative
};

// B flags
// https://wiki.nesdev.org/w/index.php?title=Status_flags#The_B_flag
static uint8_t cpu_status_interrupt_b = 0b00100000;
static uint8_t cpu_status_instruction_b = 0b00110000;

void status_set(struct nes *nes, enum cpu_status s, bool v) {
  nes->cpu->P &= ~(1 << s);
  nes->cpu->P |= v << s;
}

bool is_status_on(struct nes *nes, enum cpu_status s) {
  return ((nes->cpu->P >> s) & 1) == 1;
}

void status_set_zn(struct nes *nes, uint8_t v) {
  status_set(nes, Z, v == 0);
  status_set(nes, N, (v & 0x80) == 0x80);
}

void push_stack(struct nes *nes, uint8_t v) {
  cpu_write(nes, nes->cpu->S + 0x0100, v);
  nes->cpu->S--;
}

void push_stack_word(struct nes *nes, uint16_t v) {
  push_stack(nes, v >> 8);
  push_stack(nes, v & 0xFF);
}

uint8_t cpu_pull_stack(struct nes *nes) {
  nes->cpu->S++;
  return cpu_read(nes, nes->cpu->S + 0x0100);
}

uint16_t cpu_pull_stack_word(struct nes *nes) {
  return (uint16_t)cpu_pull_stack(nes) | ((uint16_t)cpu_pull_stack(nes) << 8);
}

void and (struct nes * nes, uint16_t v);
void eor(struct nes *nes, uint16_t v);
void ora(struct nes *nes, uint16_t v);
void abc(struct nes *nes, uint16_t v);
void sbc(struct nes *nes, uint16_t v);
void cmp(struct nes *nes, uint8_t m, uint16_t v);
void asl(struct nes *nes, uint8_t *m);
void lsr(struct nes *nes, uint8_t *m);
void rol(struct nes *nes, uint8_t *m);
void ror(struct nes *nes, uint8_t *m);
void branch(struct nes *nes, uint8_t v);

void set_carry_status(struct nes *nes, uint8_t m, uint8_t r);

void execute(struct nes *nes, struct instruction inst) {
  uint16_t operand = cpu_get_operand(nes, inst.mode);

  switch (inst.mnemonic) {
  case LDA:
    nes->cpu->A = cpu_read(nes, operand);
    status_set_zn(nes, nes->cpu->A);
  case LDX:
    nes->cpu->X = cpu_read(nes, operand);
    status_set_zn(nes, nes->cpu->X);
  case LDY:
    nes->cpu->Y = cpu_read(nes, operand);
    status_set_zn(nes, nes->cpu->Y);
  case STA:
    cpu_write(nes, operand, nes->cpu->A);
  case STX:
    cpu_write(nes, operand, nes->cpu->X);
  case STY:
    cpu_write(nes, operand, nes->cpu->Y);
  case TAX:
    nes->cpu->X = nes->cpu->A;
    status_set_zn(nes, nes->cpu->X);
    cpu_tick(nes);
  case TAY:
    nes->cpu->Y = nes->cpu->A;
    status_set_zn(nes, nes->cpu->Y);
    cpu_tick(nes);
  case TXA:
    nes->cpu->A = nes->cpu->X;
    status_set_zn(nes, nes->cpu->A);
    cpu_tick(nes);
  case TYA:
    nes->cpu->A = nes->cpu->Y;
    status_set_zn(nes, nes->cpu->A);
    cpu_tick(nes);
  case TSX:
    nes->cpu->X = nes->cpu->S;
    status_set_zn(nes, nes->cpu->X);
    cpu_tick(nes);
  case TXS:
    nes->cpu->S = nes->cpu->X;
    cpu_tick(nes);

  case PHA:
    push_stack(nes, nes->cpu->A);
    cpu_tick(nes);
  case PHP:
    push_stack(nes, nes->cpu->P | cpu_status_instruction_b);
    cpu_tick(nes);
  case PLA:
    nes->cpu->A = cpu_pull_stack(nes);
    status_set_zn(nes, nes->cpu->A);
    cpu_tick(nes);
    cpu_tick(nes);
  case PLP:
    nes->cpu->P = (cpu_pull_stack(nes) & ~cpu_status_instruction_b) |
                  0b100000; // for nestest
    cpu_tick(nes);
    cpu_tick(nes);

  case AND:
    and(nes, operand);
  case EOR:
    eor(nes, operand);
  case ORA:
    ora(nes, operand);
  case BIT: {
    uint8_t m = cpu_read(nes, operand);
    uint8_t b = nes->cpu->A & m;
    nes->cpu->P |= (b == 0) << Z;
    nes->cpu->P |= ((m & 0x40) == 0x40) << V;
    nes->cpu->P |= ((m & 0x80) == 0x80) << N;
  }

  case ADC: {
    uint8_t m = cpu_read(nes, operand);
    uint8_t r = nes->cpu->A + m;
    if (nes->cpu->P >> C)
      r++;
    set_carry_status(nes, m, r);
    nes->cpu->A = r;
    status_set_zn(nes, nes->cpu->A);
  }
  case SBC:
    sbc(nes, operand);
  case CMP:
    cmp(nes, nes->cpu->A, operand);
  case CPX:
    cmp(nes, nes->cpu->X, operand);
  case CPY:
    cmp(nes, nes->cpu->Y, operand);

  case INC: {
    uint8_t r = cpu_read(nes, operand) + 1;
    cpu_write(nes, operand, r);
    status_set_zn(nes, r);
    cpu_tick(nes);
  }
  case INX:
    nes->cpu->X++;
    status_set_zn(nes, nes->cpu->X);
    cpu_tick(nes);
  case INY:
    nes->cpu->Y++;
    status_set_zn(nes, nes->cpu->Y);
    cpu_tick(nes);
  case DEC: {
    uint8_t r = cpu_read(nes, operand) - 1;
    cpu_write(nes, operand, r);
    status_set_zn(nes, r);
    cpu_tick(nes);
  }
  case DEX:
    nes->cpu->X--;
    status_set_zn(nes, nes->cpu->X);
    cpu_tick(nes);
  case DEY:
    nes->cpu->Y--;
    status_set_zn(nes, nes->cpu->Y);
    cpu_tick(nes);

  case ASL:
    if (inst.mode == accumulator) {
      asl(nes, &nes->cpu->A);
    } else {
      uint8_t m = cpu_read(nes, operand);
      asl(nes, &m);
      cpu_write(nes, operand, m);
    }
  case LSR:
    if (inst.mode == accumulator) {
      lsr(nes, &nes->cpu->A);
    } else {
      uint8_t m = cpu_read(nes, operand);
      lsr(nes, &m);
      cpu_write(nes, operand, m);
    }
  case ROL:
    if (inst.mode == accumulator) {
      rol(nes, &nes->cpu->A);
    } else {
      uint8_t m = cpu_read(nes, operand);
      rol(nes, &m);
      cpu_write(nes, operand, m);
    }
  case ROR:
    if (inst.mode == accumulator) {
      ror(nes, &nes->cpu->A);
    } else {
      uint8_t m = cpu_read(nes, operand);
      ror(nes, &m);
      cpu_write(nes, operand, m);
    }

  case JMP:
    nes->cpu->PC = operand;
  case JSR:
    push_stack_word(nes, nes->cpu->PC - 1);
    nes->cpu->PC = operand;
    cpu_tick(nes);
  case RTS:
    nes->cpu->PC = cpu_pull_stack_word(nes);
    nes->cpu->PC++;
    cpu_tick(nes);
    cpu_tick(nes);
    cpu_tick(nes);

  case BCC:
    if (!is_status_on(nes, C))
      branch(nes, operand);
  case BCS:
    if (is_status_on(nes, C))
      branch(nes, operand);
  case BEQ:
    if (is_status_on(nes, Z))
      branch(nes, operand);
  case BMI:
    if (is_status_on(nes, N))
      branch(nes, operand);
  case BNE:
    if (!is_status_on(nes, Z))
      branch(nes, operand);
  case BPL:
    if (!is_status_on(nes, N))
      branch(nes, operand);
  case BVC:
    if (!is_status_on(nes, V))
      branch(nes, operand);
  case BVS:
    if (is_status_on(nes, V))
      branch(nes, operand);

  case CLC:
    status_set(nes, C, false);
    cpu_tick(nes);
  case CLD:
    status_set(nes, D, false);
    cpu_tick(nes);
  case CLI:
    status_set(nes, I, false);
    cpu_tick(nes);
  case CLV:
    status_set(nes, V, false);
    cpu_tick(nes);
  case SEC:
    status_set(nes, C, true);
    cpu_tick(nes);
  case SED:
    status_set(nes, D, true);
    cpu_tick(nes);
  case SEI:
    status_set(nes, I, true);
    cpu_tick(nes);

  case BRK:
    push_stack_word(nes, nes->cpu->PC);
    nes->cpu->P |= cpu_status_instruction_b;
    push_stack(nes, nes->cpu->P);
    nes->cpu->PC = cpu_read_word(nes, 0xFFFE);
    cpu_tick(nes);
  case NOP:
    cpu_tick(nes);
  case RTI:
    nes->cpu->P |= cpu_pull_stack(nes);
    nes->cpu->PC = cpu_pull_stack_word(nes);
    cpu_tick(nes);
    cpu_tick(nes);

  case LAX: {
    uint8_t m = cpu_read(nes, operand);
    nes->cpu->A = m;
    status_set_zn(nes, m);
    nes->cpu->X = m;
  }
  case SAX:
    cpu_write(nes, operand, nes->cpu->A & nes->cpu->X);
  case DCP:
    // decrementMemory excluding tick
    {
      uint8_t m = cpu_read(nes, operand) - 1;
      status_set_zn(nes, m);
      cpu_write(nes, operand, m);
      cmp(nes, nes->cpu->A, operand);
    }
  case ISB:
    // incrementMemory excluding tick
    {
      uint8_t m = cpu_read(nes, operand) - 1;
      status_set_zn(nes, m);
      cpu_write(nes, operand, m);
      sbc(nes, operand);
    }
  case SLO:
    // arithmeticShiftLeft excluding tick
    {
      uint8_t m = cpu_read(nes, operand);
      status_set(nes, C, (m & 0x80) == 0x80);
      m <<= 1;
      cpu_write(nes, operand, m);
      ora(nes, operand);
    }
  case RLA:
    // rotateLeft excluding tick
    {
      uint8_t m = cpu_read(nes, operand);
      uint8_t carry = m & 0x80;
      m <<= 1;
      if (is_status_on(nes, C)) {
        m |= 1;
      }
      status_set(nes, C, carry == 0x80);
      status_set_zn(nes, m);
      cpu_write(nes, operand, m);
      and(nes, operand);
    }
  case SRE:
    // logicalShiftRight excluding tick
    {
      uint8_t m = cpu_read(nes, operand);
      status_set(nes, C, (m & 1) == 1);
      m >>= 1;
      status_set_zn(nes, m);
      cpu_write(nes, operand, m);
      eor(nes, operand);
    }
  case RRA:
    // rotateRight excluding tick
    {
      uint8_t m = cpu_read(nes, operand);
      uint8_t carry = m & 1;
      m >>= 1;
      if (is_status_on(nes, C)) {
        m |= 0x80;
      }
      status_set(nes, C, carry == 1);
      status_set_zn(nes, m);
      cpu_write(nes, operand, m);
      abc(nes, operand);
    }
    /* default: */
    /*     panic(fmt.Sprintf("unrecognized mnemonic: %d", inst.Mnemonic)) */
  }
}

void and (struct nes * nes, uint16_t v) {
  nes->cpu->A &= cpu_read(nes, v);
  status_set_zn(nes, nes->cpu->A);
}

void eor(struct nes *nes, uint16_t v) {
  nes->cpu->A ^= cpu_read(nes, v);
  status_set_zn(nes, nes->cpu->A);
}

void ora(struct nes *nes, uint16_t v) {
  nes->cpu->A |= cpu_read(nes, v);
  status_set_zn(nes, nes->cpu->A);
}

void abc(struct nes *nes, uint16_t v) {
  uint8_t m = cpu_read(nes, v);
  uint8_t r = nes->cpu->A + m;
  if (is_status_on(nes, C)) {
    r++;
  }
  set_carry_status(nes, m, r);
  nes->cpu->A = r;
  status_set_zn(nes, nes->cpu->A);
}

void sbc(struct nes *nes, uint16_t v) {
  uint8_t m = ~cpu_read(nes, v);
  uint8_t r = nes->cpu->A + m;
  if (is_status_on(nes, C)) {
    r++;
  }
  set_carry_status(nes, m, r);
  nes->cpu->A = r;
  status_set_zn(nes, nes->cpu->A);
}

void cmp(struct nes *nes, uint8_t m, uint16_t v) {
  int16_t r = (int16_t)m - (int16_t)cpu_read(nes, v);
  status_set_zn(nes, r);
  status_set(nes, C, 0 <= r);
}

void asl(struct nes *nes, uint8_t *m) {
  status_set(nes, C, (*m & 0x80) == 0x80);
  *m <<= 1;
  status_set_zn(nes, *m);
  cpu_tick(nes);
}

void lsr(struct nes *nes, uint8_t *m) {
  status_set(nes, C, (*m & 1) == 1);
  *m >>= 1;
  status_set_zn(nes, *m);
  cpu_tick(nes);
}

void rol(struct nes *nes, uint8_t *m) {
  uint8_t carry = *m & 0x80;
  *m <<= 1;
  if (is_status_on(nes, C)) {
    *m |= 1;
  }
  status_set(nes, C, carry == 0x80);
  status_set_zn(nes, *m);
  cpu_tick(nes);
}

void ror(struct nes *nes, uint8_t *m) {
  uint8_t carry = *m & 1;
  *m >>= 1;
  if (is_status_on(nes, C)) {
    *m |= 0x80;
  }
  status_set(nes, C, carry == 1);
  status_set_zn(nes, *m);
  cpu_tick(nes);
}

void branch(struct nes *nes, uint8_t v) {
  cpu_tick(nes);
  uint16_t base = nes->cpu->PC;
  int8_t offset = v; // to negative number
  if (is_page_crossed(offset, base)) {
    cpu_tick(nes);
  }
  nes->cpu->PC = base + (int16_t)offset;
}

void set_carry_status(struct nes *nes, uint8_t m, uint8_t r) {
  uint8_t a7 = nes->cpu->A >> 7 & 1;
  uint8_t m7 = m >> 7 & 1;
  uint8_t c6 = a7 ^ m7 ^ (r >> 7 & 1);
  uint8_t c7 = (a7 & m7) | (a7 & c6) | (m7 & c6);
  status_set(nes, C, c7 == 1);
  status_set(nes, V, (c6 ^ c7) == 1);
}
