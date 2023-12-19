#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "cpu.h"
#include "cpu_emu.h"
#include "cpu_decoder.h"

#include "nes_emu.h"

#include "mem.h"

void cpu_tick(NES *nes) {
  nes_tick(nes);
  nes->cpu.cycles++;
}

uint8_t cpu_read(NES *nes, uint16_t addr) {
  uint8_t m = mem_read(nes, addr);
  cpu_tick(nes);
  return m;
}

uint16_t cpu_read_word(NES *nes, uint16_t addr) {
  return cpu_read(nes, addr) | cpu_read(nes, addr + 1) << 8;
}

void cpu_write(NES *nes, uint16_t addr, uint8_t val) {
  mem_write(nes, addr, val);
  cpu_tick(nes);
}

void cpu_power_on(NES *nes) {
  // https://wiki.nesdev.com/w/index.php/CPU_power_up_state
  nes->cpu.A = 0x00;
  nes->cpu.X = 0x00;
  nes->cpu.Y = 0x00;
  nes->cpu.S = 0xFD;
  nes->cpu.P = 0x34;            // IRQ disabled
  cpu_write(nes, 0x4017, 0x00); // frame irq disabled
  cpu_write(nes, 0x4015, 0x00); // all channels disabled
}

void cpu_reset(NES *nes) {
  nes->cpu.PC = cpu_read_word(nes, 0xFFFC);
  printf("initial PC: %x\n", nes->cpu.PC);
  nes->cpu.P = CPU_STATUS_I;
  nes->cpu.S -= 3;
}

void cpu_execute(NES *nes, CPUInstruction inst);

void handle_interrupt(NES *nes);

void cpu_step(NES *nes) {
  handle_interrupt(nes);

  uint8_t op = cpu_read(nes, nes->cpu.PC);
  nes->cpu.PC++;

  CPUInstruction inst = cpu_decode(op);
  cpu_execute(nes, inst);
}

void push_stack(NES *nes, uint8_t v);
void push_stack_word(NES *nes, uint16_t v);

// B flags
// https://wiki.nesdev.org/w/index.php?title=Status_flags#The_B_flag
static uint8_t cpu_status_interrupt_b = 0b00100000;
static uint8_t cpu_status_instruction_b = 0b00110000;

void handle_interrupt(NES *nes) {
  uint16_t vector;
  switch (nes->interrupt) {
  case INTERRUPT_NONE:
    return; // not interrupted
  case INTERRUPT_NMI:
    vector = 0xFFFA;
    break; // interrupted
  case INTERRUPT_IRQ:
    if (!cpu_status_enabled(&nes->cpu, CPU_STATUS_I)) {
      return; // not interrupted
    }
    vector = 0xFFFE;
    break; // interrupted
  }

  cpu_tick(nes);
  cpu_tick(nes);
  push_stack_word(nes, nes->cpu.PC);
  // https://wiki.nesdev.com/w/index.php/Status_flags#The_B_flag
  // http://visual6502.org/wiki/index.php?title=6502_BRK_and_B_bit
  push_stack(nes, nes->cpu.P | cpu_status_interrupt_b);
  cpu_status_set(&nes->cpu, CPU_STATUS_I, true);
  nes->cpu.PC = cpu_read_word(nes, vector);
  nes->interrupt = INTERRUPT_NONE;
}

uint16_t read_on_indirect(NES *nes, uint16_t addr) {
  uint16_t low = (uint16_t)cpu_read(nes, addr);
  // Reproduce 6502 bug - http://nesdev.com/6502bugs.txt
  uint16_t high =
      (uint16_t)cpu_read(nes, (addr & 0xFF00) | ((addr + 1) & 0x00FF));
  return low | (high << 8);
}

bool is_page_crossed(uint16_t a, uint16_t b) {
  return ((a + b) & 0xFF00) != (b & 0xFF00);
}

uint16_t cpu_get_operand(NES *nes, AddressingMode mode) {
  switch (mode) {
  case IMPLICIT:
    return 0;
  case ACCUMULATOR:
    return nes->cpu.A;
  case IMMEDIATE:
    return nes->cpu.PC++;
  case ZERO_PAGE: {
    uint16_t value = cpu_read(nes, nes->cpu.PC);
    nes->cpu.PC++;
    return value;
  }
  case ZERO_PAGE_X: {
    cpu_tick(nes);
    uint16_t value = (cpu_read(nes, nes->cpu.PC) + nes->cpu.X) & 0xFF;
    nes->cpu.PC++;
    return value;
  }
  case ZERO_PAGE_Y: {
    cpu_tick(nes);
    uint16_t value = (cpu_read(nes, nes->cpu.PC) + nes->cpu.Y) & 0xFF;
    nes->cpu.PC++;
    return value;
  }
  case ABSOLUTE: {
    uint16_t value = cpu_read_word(nes, nes->cpu.PC);
    nes->cpu.PC += 2;
    return value;
  }
  case ABSOLUTE_X: {
    uint16_t value = cpu_read_word(nes, nes->cpu.PC);
    nes->cpu.PC += 2;
    cpu_tick(nes);
    return value + nes->cpu.X;
  }
  case ABSOLUTE_X_WITH_PENALTY: {
    uint16_t value = cpu_read_word(nes, nes->cpu.PC);
    nes->cpu.PC += 2;
    if (is_page_crossed(nes->cpu.X, value)) {
      cpu_tick(nes);
    }
    return value + nes->cpu.X;
  }
  case ABSOLUTE_Y: {
    uint16_t value = cpu_read_word(nes, nes->cpu.PC);
    nes->cpu.PC += 2;
    cpu_tick(nes);
    return value + nes->cpu.Y;
  }
  case ABSOLUTE_Y_WITH_PENALTY: {
    uint16_t value = cpu_read_word(nes, nes->cpu.PC);
    nes->cpu.PC += 2;
    if (is_page_crossed(nes->cpu.Y, value)) {
      cpu_tick(nes);
    }
    return value + nes->cpu.Y;
  }
  case RELATIVE: {
    uint16_t value = cpu_read(nes, nes->cpu.PC);
    nes->cpu.PC++;
    return value;
  }
  case INDIRECT: {
    uint16_t m = cpu_read_word(nes, nes->cpu.PC);
    uint16_t value = read_on_indirect(nes, m);
    nes->cpu.PC += 2;
    return value;
  }
  case INDEXED_INDIRECT: {
    uint8_t m = cpu_read(nes, nes->cpu.PC);
    uint16_t value = read_on_indirect(nes, (uint8_t)(m + nes->cpu.X));
    nes->cpu.PC += 1;
    cpu_tick(nes);
    return value;
  }
  case INDIRECT_INDEXED: {
    uint8_t m = cpu_read(nes, nes->cpu.PC);
    uint16_t value = read_on_indirect(nes, m);
    nes->cpu.PC++;
    cpu_tick(nes);
    return value + nes->cpu.Y;
  }
  case INDIRECT_INDEXED_WITH_PENALTY: {
    uint8_t m = cpu_read(nes, nes->cpu.PC);
    uint16_t value = read_on_indirect(nes, m);
    nes->cpu.PC++;
    if (is_page_crossed(nes->cpu.Y, value)) {
      cpu_tick(nes);
    }
    return value + nes->cpu.Y;
  }
  }
  return 0;
}

void push_stack(NES *nes, uint8_t v) {
  cpu_write(nes, nes->cpu.S + 0x0100, v);
  nes->cpu.S--;
}

void push_stack_word(NES *nes, uint16_t v) {
  push_stack(nes, v >> 8);
  push_stack(nes, v & 0xFF);
}

uint8_t cpu_pull_stack(NES *nes) {
  nes->cpu.S++;
  return cpu_read(nes, nes->cpu.S + 0x0100);
}

uint16_t cpu_pull_stack_word(NES *nes) {
  return (uint16_t)cpu_pull_stack(nes) | ((uint16_t)cpu_pull_stack(nes) << 8);
}

void and (NES * nes, uint16_t v);
void eor(NES *nes, uint16_t v);
void ora(NES *nes, uint16_t v);
void abc(NES *nes, uint16_t v);
void sbc(NES *nes, uint16_t v);
void cmp(NES *nes, uint8_t m, uint16_t v);
void asl(NES *nes, uint8_t *m);
void lsr(NES *nes, uint8_t *m);
void rol(NES *nes, uint8_t *m);
void ror(NES *nes, uint8_t *m);
void branch(NES *nes, uint8_t v);

void set_carry_status(NES *nes, uint8_t m, uint8_t r);

void cpu_execute(NES *nes, CPUInstruction inst) {
  uint16_t operand = cpu_get_operand(nes, inst.mode);

  switch (inst.mnemonic) {
  case LDA:
    nes->cpu.A = cpu_read(nes, operand);
    cpu_status_set_zn(&nes->cpu, nes->cpu.A);
    break;
  case LDX:
    nes->cpu.X = cpu_read(nes, operand);
    cpu_status_set_zn(&nes->cpu, nes->cpu.X);
    break;
  case LDY:
    nes->cpu.Y = cpu_read(nes, operand);
    cpu_status_set_zn(&nes->cpu, nes->cpu.Y);
    break;
  case STA:
    cpu_write(nes, operand, nes->cpu.A);
    break;
  case STX:
    cpu_write(nes, operand, nes->cpu.X);
    break;
  case STY:
    cpu_write(nes, operand, nes->cpu.Y);
    break;
  case TAX:
    nes->cpu.X = nes->cpu.A;
    cpu_status_set_zn(&nes->cpu, nes->cpu.X);
    cpu_tick(nes);
    break;
  case TAY:
    nes->cpu.Y = nes->cpu.A;
    cpu_status_set_zn(&nes->cpu, nes->cpu.Y);
    cpu_tick(nes);
    break;
  case TXA:
    nes->cpu.A = nes->cpu.X;
    cpu_status_set_zn(&nes->cpu, nes->cpu.A);
    cpu_tick(nes);
    break;
  case TYA:
    nes->cpu.A = nes->cpu.Y;
    cpu_status_set_zn(&nes->cpu, nes->cpu.A);
    cpu_tick(nes);
    break;
  case TSX:
    nes->cpu.X = nes->cpu.S;
    cpu_status_set_zn(&nes->cpu, nes->cpu.X);
    cpu_tick(nes);
    break;
  case TXS:
    nes->cpu.S = nes->cpu.X;
    cpu_tick(nes);
    break;

  case PHA:
    push_stack(nes, nes->cpu.A);
    cpu_tick(nes);
    break;
  case PHP:
    push_stack(nes, nes->cpu.P | cpu_status_instruction_b);
    cpu_tick(nes);
    break;
  case PLA:
    nes->cpu.A = cpu_pull_stack(nes);
    cpu_status_set_zn(&nes->cpu, nes->cpu.A);
    cpu_tick(nes);
    cpu_tick(nes);
    break;
  case PLP: {
    nes->cpu.P = cpu_pull_stack(nes) & ~cpu_status_instruction_b;
    nes->cpu.P |= 0b00100000; // for nestest
    cpu_tick(nes);
    cpu_tick(nes);
    break;
  }

  case AND:
    and(nes, operand);
    break;
  case EOR:
    eor(nes, operand);
    break;
  case ORA:
    ora(nes, operand);
    break;
  case BIT: {
    uint8_t m = cpu_read(nes, operand);
    uint8_t b = nes->cpu.A & m;
    cpu_status_set(&nes->cpu, CPU_STATUS_Z, b == 0);
    cpu_status_set(&nes->cpu, CPU_STATUS_V, (m & 0x40) == 0x40);
    cpu_status_set(&nes->cpu, CPU_STATUS_N, (m & 0x80) == 0x80);
    break;
  }

  case ADC: {
    uint8_t m = cpu_read(nes, operand);
    uint8_t r = nes->cpu.A + m;
    if (cpu_status_enabled(&nes->cpu, CPU_STATUS_C))
      r++;
    set_carry_status(nes, m, r);
    nes->cpu.A = r;
    cpu_status_set_zn(&nes->cpu, nes->cpu.A);
    break;
  }
  case SBC:
    sbc(nes, operand);
    break;
  case CMP:
    cmp(nes, nes->cpu.A, operand);
    break;
  case CPX:
    cmp(nes, nes->cpu.X, operand);
    break;
  case CPY:
    cmp(nes, nes->cpu.Y, operand);
    break;

  case INC: {
    uint8_t r = cpu_read(nes, operand) + 1;
    cpu_write(nes, operand, r);
    cpu_status_set_zn(&nes->cpu, r);
    cpu_tick(nes);
    break;
  }
  case INX:
    nes->cpu.X++;
    cpu_status_set_zn(&nes->cpu, nes->cpu.X);
    cpu_tick(nes);
    break;
  case INY:
    nes->cpu.Y++;
    cpu_status_set_zn(&nes->cpu, nes->cpu.Y);
    cpu_tick(nes);
    break;
  case DEC: {
    uint8_t r = cpu_read(nes, operand) - 1;
    cpu_write(nes, operand, r);
    cpu_status_set_zn(&nes->cpu, r);
    cpu_tick(nes);
    break;
  }
  case DEX:
    nes->cpu.X--;
    cpu_status_set_zn(&nes->cpu, nes->cpu.X);
    cpu_tick(nes);
    break;
  case DEY:
    nes->cpu.Y--;
    cpu_status_set_zn(&nes->cpu, nes->cpu.Y);
    cpu_tick(nes);
    break;

  case ASL:
    if (inst.mode == ACCUMULATOR) {
      asl(nes, &nes->cpu.A);
    } else {
      uint8_t m = cpu_read(nes, operand);
      asl(nes, &m);
      cpu_write(nes, operand, m);
    }
    break;
  case LSR:
    if (inst.mode == ACCUMULATOR) {
      lsr(nes, &nes->cpu.A);
    } else {
      uint8_t m = cpu_read(nes, operand);
      lsr(nes, &m);
      cpu_write(nes, operand, m);
    }
    break;
  case ROL:
    if (inst.mode == ACCUMULATOR) {
      rol(nes, &nes->cpu.A);
    } else {
      uint8_t m = cpu_read(nes, operand);
      rol(nes, &m);
      cpu_write(nes, operand, m);
    }
    break;
  case ROR:
    if (inst.mode == ACCUMULATOR) {
      ror(nes, &nes->cpu.A);
    } else {
      uint8_t m = cpu_read(nes, operand);
      ror(nes, &m);
      cpu_write(nes, operand, m);
    }
    break;

  case JMP:
    nes->cpu.PC = operand;
    break;
  case JSR:
    push_stack_word(nes, nes->cpu.PC - 1);
    nes->cpu.PC = operand;
    cpu_tick(nes);
    break;
  case RTS:
    nes->cpu.PC = cpu_pull_stack_word(nes);
    nes->cpu.PC++;
    cpu_tick(nes);
    cpu_tick(nes);
    cpu_tick(nes);
    break;

  case BCC:
    if (!cpu_status_enabled(&nes->cpu, CPU_STATUS_C))
      branch(nes, operand);
    break;
  case BCS:
    if (cpu_status_enabled(&nes->cpu, CPU_STATUS_C))
      branch(nes, operand);
    break;
  case BEQ:
    if (cpu_status_enabled(&nes->cpu, CPU_STATUS_Z))
      branch(nes, operand);
    break;
  case BMI:
    if (cpu_status_enabled(&nes->cpu, CPU_STATUS_N))
      branch(nes, operand);
    break;
  case BNE:
    if (!cpu_status_enabled(&nes->cpu, CPU_STATUS_Z))
      branch(nes, operand);
    break;
  case BPL:
    if (!cpu_status_enabled(&nes->cpu, CPU_STATUS_N))
      branch(nes, operand);
    break;
  case BVC:
    if (!cpu_status_enabled(&nes->cpu, CPU_STATUS_V))
      branch(nes, operand);
    break;
  case BVS:
    if (cpu_status_enabled(&nes->cpu, CPU_STATUS_V))
      branch(nes, operand);
    break;

  case CLC:
    cpu_status_set(&nes->cpu, CPU_STATUS_C, false);
    cpu_tick(nes);
    break;
  case CLD:
    cpu_status_set(&nes->cpu, CPU_STATUS_D, false);
    cpu_tick(nes);
    break;
  case CLI:
    cpu_status_set(&nes->cpu, CPU_STATUS_I, false);
    cpu_tick(nes);
    break;
  case CLV:
    cpu_status_set(&nes->cpu, CPU_STATUS_V, false);
    cpu_tick(nes);
    break;
  case SEC:
    cpu_status_set(&nes->cpu, CPU_STATUS_C, true);
    cpu_tick(nes);
    break;
  case SED:
    cpu_status_set(&nes->cpu, CPU_STATUS_D, true);
    cpu_tick(nes);
    break;
  case SEI:
    cpu_status_set(&nes->cpu, CPU_STATUS_I, true);
    cpu_tick(nes);
    break;

  case BRK:
    push_stack_word(nes, nes->cpu.PC);
    push_stack(nes, nes->cpu.P);
    nes->cpu.P |= cpu_status_instruction_b;
    nes->cpu.PC = cpu_read_word(nes, 0xFFFE);
    cpu_tick(nes);
    break;
  case NOP:
    cpu_tick(nes);
    break;
  case RTI:
    nes->cpu.P = cpu_pull_stack(nes);
    nes->cpu.P = nes->cpu.P & ~cpu_status_instruction_b;
    nes->cpu.PC = cpu_pull_stack_word(nes);
    cpu_tick(nes);
    cpu_tick(nes);
    break;

  case LAX: {
    uint8_t m = cpu_read(nes, operand);
    nes->cpu.A = m;
    cpu_status_set_zn(&nes->cpu, m);
    nes->cpu.X = m;
    break;
  }
  case SAX:
    cpu_write(nes, operand, nes->cpu.A & nes->cpu.X);
    break;
  case DCP: {
    // decrementMemory excluding tick
    uint8_t m = cpu_read(nes, operand) - 1;
    cpu_status_set_zn(&nes->cpu, m);
    cpu_write(nes, operand, m);
    cmp(nes, nes->cpu.A, operand);
    break;
  }
  case ISB: {
    // incrementMemory excluding tick
    uint8_t m = cpu_read(nes, operand) + 1;
    cpu_status_set_zn(&nes->cpu, m);
    cpu_write(nes, operand, m);
    sbc(nes, operand);
    break;
  }
  case SLO: {
    // arithmeticShiftLeft excluding tick
    uint8_t m = cpu_read(nes, operand);
    cpu_status_set(&nes->cpu, CPU_STATUS_C, (m & 0x80) == 0x80);
    m <<= 1;
    cpu_write(nes, operand, m);
    ora(nes, operand);
    break;
  }
  case RLA: {
    // rotateLeft excluding tick
    uint8_t m = cpu_read(nes, operand);
    uint8_t carry = m & 0x80;
    m <<= 1;
    if (cpu_status_enabled(&nes->cpu, CPU_STATUS_C)) {
      m |= 1;
    }
    cpu_status_set(&nes->cpu, CPU_STATUS_C, carry == 0x80);
    cpu_status_set_zn(&nes->cpu, m);
    cpu_write(nes, operand, m);
    and(nes, operand);
    break;
  }
  case SRE: {
    // logicalShiftRight excluding tick
    uint8_t m = cpu_read(nes, operand);
    cpu_status_set(&nes->cpu, CPU_STATUS_C, (m & 1) == 1);
    m >>= 1;
    cpu_status_set_zn(&nes->cpu, m);
    cpu_write(nes, operand, m);
    eor(nes, operand);
    break;
  }
  case RRA: {
    // rotateRight excluding tick
    uint8_t m = cpu_read(nes, operand);
    uint8_t carry = m & 1;
    m >>= 1;
    if (cpu_status_enabled(&nes->cpu, CPU_STATUS_C)) {
      m |= 0x80;
    }
    cpu_status_set(&nes->cpu, CPU_STATUS_C, carry == 1);
    cpu_status_set_zn(&nes->cpu, m);
    cpu_write(nes, operand, m);
    abc(nes, operand);
    break;
  }
    /* default: */
    /*     panic(fmt.Sprintf("unrecognized mnemonic: %d", inst.Mnemonic)) */
  }
}

void and (NES * nes, uint16_t v) {
  nes->cpu.A &= cpu_read(nes, v);
  cpu_status_set_zn(&nes->cpu, nes->cpu.A);
}

void eor(NES *nes, uint16_t v) {
  nes->cpu.A ^= cpu_read(nes, v);
  cpu_status_set_zn(&nes->cpu, nes->cpu.A);
}

void ora(NES *nes, uint16_t v) {
  nes->cpu.A |= cpu_read(nes, v);
  cpu_status_set_zn(&nes->cpu, nes->cpu.A);
}

void abc(NES *nes, uint16_t v) {
  uint8_t m = cpu_read(nes, v);
  uint8_t r = nes->cpu.A + m;
  if (cpu_status_enabled(&nes->cpu, CPU_STATUS_C)) {
    r++;
  }
  set_carry_status(nes, m, r);
  nes->cpu.A = r;
  cpu_status_set_zn(&nes->cpu, nes->cpu.A);
}

void sbc(NES *nes, uint16_t v) {
  uint8_t m = ~cpu_read(nes, v);
  uint8_t r = nes->cpu.A + m;
  if (cpu_status_enabled(&nes->cpu, CPU_STATUS_C)) {
    r++;
  }
  set_carry_status(nes, m, r);
  nes->cpu.A = r;
  cpu_status_set_zn(&nes->cpu, nes->cpu.A);
}

void cmp(NES *nes, uint8_t m, uint16_t v) {
  int16_t r = (int16_t)m - (int16_t)cpu_read(nes, v);
  cpu_status_set_zn(&nes->cpu, r);
  cpu_status_set(&nes->cpu, CPU_STATUS_C, 0 <= r);
}

void asl(NES *nes, uint8_t *m) {
  cpu_status_set(&nes->cpu, CPU_STATUS_C, (*m & 0x80) == 0x80);
  *m <<= 1;
  cpu_status_set_zn(&nes->cpu, *m);
  cpu_tick(nes);
}

void lsr(NES *nes, uint8_t *m) {
  cpu_status_set(&nes->cpu, CPU_STATUS_C, (*m & 1) == 1);
  *m >>= 1;
  cpu_status_set_zn(&nes->cpu, *m);
  cpu_tick(nes);
}

void rol(NES *nes, uint8_t *m) {
  uint8_t carry = *m & 0x80;
  *m <<= 1;
  if (cpu_status_enabled(&nes->cpu, CPU_STATUS_C)) {
    *m |= 1;
  }
  cpu_status_set(&nes->cpu, CPU_STATUS_C, carry == 0x80);
  cpu_status_set_zn(&nes->cpu, *m);
  cpu_tick(nes);
}

void ror(NES *nes, uint8_t *m) {
  uint8_t carry = *m & 1;
  *m >>= 1;
  if (cpu_status_enabled(&nes->cpu, CPU_STATUS_C)) {
    *m |= 0x80;
  }
  cpu_status_set(&nes->cpu, CPU_STATUS_C, carry == 1);
  cpu_status_set_zn(&nes->cpu, *m);
  cpu_tick(nes);
}

void branch(NES *nes, uint8_t v) {
  cpu_tick(nes);
  int16_t base = (int16_t)nes->cpu.PC;
  int8_t offset = (int8_t)v; // to negative number
  if (is_page_crossed(offset, base)) {
    cpu_tick(nes);
  }
  nes->cpu.PC = base + (int16_t)offset;
}

void set_carry_status(NES *nes, uint8_t m, uint8_t r) {
  uint8_t a7 = nes->cpu.A >> 7 & 1;
  uint8_t m7 = m >> 7 & 1;
  uint8_t c6 = a7 ^ m7 ^ (r >> 7 & 1);
  uint8_t c7 = (a7 & m7) | (a7 & c6) | (m7 & c6);
  cpu_status_set(&nes->cpu, CPU_STATUS_C, c7 == 1);
  cpu_status_set(&nes->cpu, CPU_STATUS_V, (c6 ^ c7) == 1);
}
