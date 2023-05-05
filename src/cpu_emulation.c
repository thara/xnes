#include <stdint.h>
#include <stdbool.h>

#include "cpu_emulation.h"

#include "cpu_instruction.h"
#include "mem.h"

void cpu_tick(nes *nes) {
  nes_tick(nes);
  nes->cpu.cycles++;
}

uint8_t cpu_read(nes *nes, uint16_t addr) {
  uint8_t m = mem_read(nes, addr);
  cpu_tick(nes);
  return m;
}

void cpu_write(nes *nes, uint16_t addr, uint8_t val) {
  mem_write(nes, addr, val);
  cpu_tick(nes);
}

void cpu_power_on(nes *nes) {
  // https://wiki.nesdev.com/w/index.php/CPU_power_up_state
  nes->cpu.A = 0x00;
  nes->cpu.X = 0x00;
  nes->cpu.Y = 0x00;
  nes->cpu.S = 0xFD;
  nes->cpu.P = 0x34;            // IRQ disabled
  cpu_write(nes, 0x4017, 0x00); // frame irq disabled
  cpu_write(nes, 0x4015, 0x00); // all channels disabled
}

void cpu_execute(nes *nes, cpu_instruction inst);

void cpu_step(nes *nes) {
  uint8_t op = cpu_read(nes, nes->cpu.PC);
  nes->cpu.PC++;

  cpu_instruction inst = cpu_decode(op);
  cpu_execute(nes, inst);
}

uint16_t cpu_read_word(nes *nes, uint16_t addr) {
  return cpu_read(nes, addr) | cpu_read(nes, addr + 1) << 8;
}

uint16_t read_on_indirect(nes *nes, uint16_t addr) {
  uint16_t low = (uint16_t)cpu_read(nes, addr);
  // Reproduce 6502 bug - http://nesdev.com/6502bugs.txt
  uint16_t high =
      (uint16_t)cpu_read(nes, (addr & 0xFF00) | ((addr + 1) & 0x00FF));
  return low | (high << 8);
}

bool is_page_crossed(uint16_t a, uint16_t b) {
  return ((a + b) & 0xFF00) != (b & 0xFF00);
}

uint16_t cpu_get_operand(nes *nes, cpu_addressing_mode mode) {
  switch (mode) {
  case implicit:
    return 0;
  case accumulator:
    return nes->cpu.A;
  case immediate:
    return nes->cpu.PC++;
  case zero_page: {
    uint16_t value = cpu_read(nes, nes->cpu.PC);
    nes->cpu.PC++;
    return value;
  }
  case zero_page_x: {
    cpu_tick(nes);
    uint16_t value = (cpu_read(nes, nes->cpu.PC) + nes->cpu.X) & 0xFF;
    nes->cpu.PC++;
    return value;
  }
  case zero_page_y: {
    cpu_tick(nes);
    uint16_t value = (cpu_read(nes, nes->cpu.PC) + nes->cpu.Y) & 0xFF;
    nes->cpu.PC++;
    return value;
  }
  case absolute: {
    uint16_t value = cpu_read_word(nes, nes->cpu.PC);
    nes->cpu.PC += 2;
    return value;
  }
  case absolute_x: {
    uint16_t value = cpu_read_word(nes, nes->cpu.PC);
    nes->cpu.PC += 2;
    cpu_tick(nes);
    return value + nes->cpu.X;
  }
  case absolute_x_with_penalty: {
    uint16_t value = cpu_read_word(nes, nes->cpu.PC);
    nes->cpu.PC += 2;
    if (is_page_crossed(nes->cpu.X, value)) {
      cpu_tick(nes);
    }
    return value + nes->cpu.X;
  }
  case absolute_y: {
    uint16_t value = cpu_read_word(nes, nes->cpu.PC);
    nes->cpu.PC += 2;
    cpu_tick(nes);
    return value + nes->cpu.Y;
  }
  case absolute_y_with_penalty: {
    uint16_t value = cpu_read_word(nes, nes->cpu.PC);
    nes->cpu.PC += 2;
    if (is_page_crossed(nes->cpu.Y, value)) {
      cpu_tick(nes);
    }
    return value + nes->cpu.Y;
  }
  case relative: {
    uint16_t value = cpu_read(nes, nes->cpu.PC);
    nes->cpu.PC++;
    return value;
  }
  case indirect: {
    uint16_t m = cpu_read_word(nes, nes->cpu.PC);
    uint16_t value = read_on_indirect(nes, m);
    nes->cpu.PC += 2;
    return value;
  }
  case indexed_indirect: {
    uint8_t m = cpu_read(nes, nes->cpu.PC);
    uint16_t value = read_on_indirect(nes, (uint8_t)(m + nes->cpu.X));
    nes->cpu.PC += 1;
    cpu_tick(nes);
    return value;
  }
  case indirect_indexed: {
    uint8_t m = cpu_read(nes, nes->cpu.PC);
    uint16_t value = read_on_indirect(nes, m);
    nes->cpu.PC++;
    cpu_tick(nes);
    return value + nes->cpu.Y;
  }
  case indirect_indexed_with_penalty: {
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

// B flags
// https://wiki.nesdev.org/w/index.php?title=Status_flags#The_B_flag
static uint8_t cpu_status_interrupt_b = 0b00100000;
static uint8_t cpu_status_instruction_b = 0b00110000;

void push_stack(nes *nes, uint8_t v) {
  cpu_write(nes, nes->cpu.S + 0x0100, v);
  nes->cpu.S--;
}

void push_stack_word(nes *nes, uint16_t v) {
  push_stack(nes, v >> 8);
  push_stack(nes, v & 0xFF);
}

uint8_t cpu_pull_stack(nes *nes) {
  nes->cpu.S++;
  return cpu_read(nes, nes->cpu.S + 0x0100);
}

uint16_t cpu_pull_stack_word(nes *nes) {
  return (uint16_t)cpu_pull_stack(nes) | ((uint16_t)cpu_pull_stack(nes) << 8);
}

void and (nes * nes, uint16_t v);
void eor(nes *nes, uint16_t v);
void ora(nes *nes, uint16_t v);
void abc(nes *nes, uint16_t v);
void sbc(nes *nes, uint16_t v);
void cmp(nes *nes, uint8_t m, uint16_t v);
void asl(nes *nes, uint8_t *m);
void lsr(nes *nes, uint8_t *m);
void rol(nes *nes, uint8_t *m);
void ror(nes *nes, uint8_t *m);
void branch(nes *nes, uint8_t v);

void set_carry_status(nes *nes, uint8_t m, uint8_t r);

void cpu_execute(nes *nes, cpu_instruction inst) {
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
    nes->cpu.P |= (b == 0) << CPU_STATUS_Z;
    nes->cpu.P |= ((m & 0x40) == 0x40) << CPU_STATUS_V;
    nes->cpu.P |= ((m & 0x80) == 0x80) << CPU_STATUS_N;
    break;
  }

  case ADC: {
    uint8_t m = cpu_read(nes, operand);
    uint8_t r = nes->cpu.A + m;
    if (nes->cpu.P >> CPU_STATUS_C)
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
    if (inst.mode == accumulator) {
      asl(nes, &nes->cpu.A);
    } else {
      uint8_t m = cpu_read(nes, operand);
      asl(nes, &m);
      cpu_write(nes, operand, m);
    }
    break;
  case LSR:
    if (inst.mode == accumulator) {
      lsr(nes, &nes->cpu.A);
    } else {
      uint8_t m = cpu_read(nes, operand);
      lsr(nes, &m);
      cpu_write(nes, operand, m);
    }
  case ROL:
    if (inst.mode == accumulator) {
      rol(nes, &nes->cpu.A);
    } else {
      uint8_t m = cpu_read(nes, operand);
      rol(nes, &m);
      cpu_write(nes, operand, m);
    }
    break;
  case ROR:
    if (inst.mode == accumulator) {
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
    nes->cpu.P |= cpu_status_instruction_b;
    push_stack(nes, nes->cpu.P);
    nes->cpu.PC = cpu_read_word(nes, 0xFFFE);
    cpu_tick(nes);
    break;
  case NOP:
    cpu_tick(nes);
    break;
  case RTI:
    nes->cpu.P = cpu_pull_stack(nes);
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
    uint8_t m = cpu_read(nes, operand) - 1;
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

void and (nes * nes, uint16_t v) {
  nes->cpu.A &= cpu_read(nes, v);
  cpu_status_set_zn(&nes->cpu, nes->cpu.A);
}

void eor(nes *nes, uint16_t v) {
  nes->cpu.A ^= cpu_read(nes, v);
  cpu_status_set_zn(&nes->cpu, nes->cpu.A);
}

void ora(nes *nes, uint16_t v) {
  nes->cpu.A |= cpu_read(nes, v);
  cpu_status_set_zn(&nes->cpu, nes->cpu.A);
}

void abc(nes *nes, uint16_t v) {
  uint8_t m = cpu_read(nes, v);
  uint8_t r = nes->cpu.A + m;
  if (cpu_status_enabled(&nes->cpu, CPU_STATUS_C)) {
    r++;
  }
  set_carry_status(nes, m, r);
  nes->cpu.A = r;
  cpu_status_set_zn(&nes->cpu, nes->cpu.A);
}

void sbc(nes *nes, uint16_t v) {
  uint8_t m = ~cpu_read(nes, v);
  uint8_t r = nes->cpu.A + m;
  if (cpu_status_enabled(&nes->cpu, CPU_STATUS_C)) {
    r++;
  }
  set_carry_status(nes, m, r);
  nes->cpu.A = r;
  cpu_status_set_zn(&nes->cpu, nes->cpu.A);
}

void cmp(nes *nes, uint8_t m, uint16_t v) {
  int16_t r = (int16_t)m - (int16_t)cpu_read(nes, v);
  cpu_status_set_zn(&nes->cpu, r);
  cpu_status_set(&nes->cpu, CPU_STATUS_C, 0 <= r);
}

void asl(nes *nes, uint8_t *m) {
  cpu_status_set(&nes->cpu, CPU_STATUS_C, (*m & 0x80) == 0x80);
  *m <<= 1;
  cpu_status_set_zn(&nes->cpu, *m);
  cpu_tick(nes);
}

void lsr(nes *nes, uint8_t *m) {
  cpu_status_set(&nes->cpu, CPU_STATUS_C, (*m & 1) == 1);
  *m >>= 1;
  cpu_status_set_zn(&nes->cpu, *m);
  cpu_tick(nes);
}

void rol(nes *nes, uint8_t *m) {
  uint8_t carry = *m & 0x80;
  *m <<= 1;
  if (cpu_status_enabled(&nes->cpu, CPU_STATUS_C)) {
    *m |= 1;
  }
  cpu_status_set(&nes->cpu, CPU_STATUS_C, carry == 0x80);
  cpu_status_set_zn(&nes->cpu, *m);
  cpu_tick(nes);
}

void ror(nes *nes, uint8_t *m) {
  uint8_t carry = *m & 1;
  *m >>= 1;
  if (cpu_status_enabled(&nes->cpu, CPU_STATUS_C)) {
    *m |= 0x80;
  }
  cpu_status_set(&nes->cpu, CPU_STATUS_C, carry == 1);
  cpu_status_set_zn(&nes->cpu, *m);
  cpu_tick(nes);
}

void branch(nes *nes, uint8_t v) {
  cpu_tick(nes);
  int16_t base = (int16_t)nes->cpu.PC;
  int8_t offset = (int8_t)v; // to negative number
  if (is_page_crossed(offset, base)) {
    cpu_tick(nes);
  }
  nes->cpu.PC = base + (int16_t)offset;
}

void set_carry_status(nes *nes, uint8_t m, uint8_t r) {
  uint8_t a7 = nes->cpu.A >> 7 & 1;
  uint8_t m7 = m >> 7 & 1;
  uint8_t c6 = a7 ^ m7 ^ (r >> 7 & 1);
  uint8_t c7 = (a7 & m7) | (a7 & c6) | (m7 & c6);
  cpu_status_set(&nes->cpu, CPU_STATUS_C, c7 == 1);
  cpu_status_set(&nes->cpu, CPU_STATUS_V, (c6 ^ c7) == 1);
}
