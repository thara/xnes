#include "cpu_step.h"

#include <stdio.h>

#include "cpu_decode.h"
#include "memory_map.h"

static void cpu_tick(nes *n) {
  /* nes_tick(n); */
  n->cpu.cycles++;
}

static uint8_t cpu_read(nes *n, uint16_t addr) {
  uint8_t m = mem_read(n, addr);
  cpu_tick(n);
  return m;
}

static uint16_t cpu_read_word(nes *n, uint16_t addr) {
  return cpu_read(n, addr) | cpu_read(n, addr + 1) << 8;
}

static void cpu_write(nes *n, uint16_t addr, uint8_t val) {
  mem_write(n, addr, val);
  cpu_tick(n);
}

void cpu_power_on(nes *n) {
  // https://wiki.nesdev.com/w/index.php/CPU_power_up_state
  n->cpu.A = 0x00;
  n->cpu.X = 0x00;
  n->cpu.Y = 0x00;
  n->cpu.S = 0xFD;
  n->cpu.P = 0x34;            // IRQ disabled
  cpu_write(n, 0x4017, 0x00); // frame irq disabled
  cpu_write(n, 0x4015, 0x00); // all channels disabled
}

void cpu_reset(nes *n) {
  n->cpu.PC = cpu_read_word(n, 0xFFFC);
  printf("initial PC: %x\n", n->cpu.PC);
  n->cpu.P = CPU_STATUS_I;
  n->cpu.S -= 3;
}

static void push_stack(nes *n, uint8_t v) {
  cpu_write(n, n->cpu.S + 0x0100, v);
  n->cpu.S--;
}

static void push_stack_word(nes *n, uint16_t v) {
  push_stack(n, v >> 8);
  push_stack(n, v & 0xFF);
}

// B flags
// https://wiki.nesdev.org/w/index.php?title=Status_flags#The_B_flag
static uint8_t cpu_status_interrupt_b = 0b00100000;
static uint8_t cpu_status_instruction_b = 0b00110000;

static void handle_interrupt(nes *n) {
  uint16_t vector;
  switch (n->interrupt) {
  case INTERRUPT_NONE:
    return; // not interrupted
  case INTERRUPT_NMI:
    vector = 0xFFFA;
    break; // interrupted
  case INTERRUPT_IRQ:
    if (!cpu_status_enabled(&n->cpu, CPU_STATUS_I)) {
      return; // not interrupted
    }
    vector = 0xFFFE;
    break; // interrupted
  }

  cpu_tick(n);
  cpu_tick(n);
  push_stack_word(n, n->cpu.PC);
  // https://wiki.nesdev.com/w/index.php/Status_flags#The_B_flag
  // http://visual6502.org/wiki/index.php?title=6502_BRK_and_B_bit
  push_stack(n, n->cpu.P | cpu_status_interrupt_b);
  cpu_status_set(&n->cpu, CPU_STATUS_I, true);
  n->cpu.PC = cpu_read_word(n, vector);
  n->interrupt = INTERRUPT_NONE;
}

uint16_t read_on_indirect(nes *n, uint16_t addr) {
  uint16_t low = (uint16_t)cpu_read(n, addr);
  // Reproduce 6502 bug - http://nesdev.com/6502bugs.txt
  uint16_t high =
      (uint16_t)cpu_read(n, (addr & 0xFF00) | ((addr + 1) & 0x00FF));
  return low | (high << 8);
}

static bool is_page_crossed(uint16_t a, uint16_t b) {
  return ((a + b) & 0xFF00) != (b & 0xFF00);
}

static uint16_t get_operand(nes *n, addressing_mode mode) {
  switch (mode) {
  case IMPLICIT:
    return 0;
  case ACCUMULATOR:
    return n->cpu.A;
  case IMMEDIATE:
    return n->cpu.PC++;
  case ZERO_PAGE: {
    uint16_t value = cpu_read(n, n->cpu.PC);
    n->cpu.PC++;
    return value;
  }
  case ZERO_PAGE_X: {
    cpu_tick(n);
    uint16_t value = (cpu_read(n, n->cpu.PC) + n->cpu.X) & 0xFF;
    n->cpu.PC++;
    return value;
  }
  case ZERO_PAGE_Y: {
    cpu_tick(n);
    uint16_t value = (cpu_read(n, n->cpu.PC) + n->cpu.Y) & 0xFF;
    n->cpu.PC++;
    return value;
  }
  case ABSOLUTE: {
    uint16_t value = cpu_read_word(n, n->cpu.PC);
    n->cpu.PC += 2;
    return value;
  }
  case ABSOLUTE_X: {
    uint16_t value = cpu_read_word(n, n->cpu.PC);
    n->cpu.PC += 2;
    cpu_tick(n);
    return value + n->cpu.X;
  }
  case ABSOLUTE_X_WITH_PENALTY: {
    uint16_t value = cpu_read_word(n, n->cpu.PC);
    n->cpu.PC += 2;
    if (is_page_crossed(n->cpu.X, value)) {
      cpu_tick(n);
    }
    return value + n->cpu.X;
  }
  case ABSOLUTE_Y: {
    uint16_t value = cpu_read_word(n, n->cpu.PC);
    n->cpu.PC += 2;
    cpu_tick(n);
    return value + n->cpu.Y;
  }
  case ABSOLUTE_Y_WITH_PENALTY: {
    uint16_t value = cpu_read_word(n, n->cpu.PC);
    n->cpu.PC += 2;
    if (is_page_crossed(n->cpu.Y, value)) {
      cpu_tick(n);
    }
    return value + n->cpu.Y;
  }
  case RELATIVE: {
    uint16_t value = cpu_read(n, n->cpu.PC);
    n->cpu.PC++;
    return value;
  }
  case INDIRECT: {
    uint16_t m = cpu_read_word(n, n->cpu.PC);
    uint16_t value = read_on_indirect(n, m);
    n->cpu.PC += 2;
    return value;
  }
  case INDEXED_INDIRECT: {
    uint8_t m = cpu_read(n, n->cpu.PC);
    uint16_t value = read_on_indirect(n, (uint8_t)(m + n->cpu.X));
    n->cpu.PC += 1;
    cpu_tick(n);
    return value;
  }
  case INDIRECT_INDEXED: {
    uint8_t m = cpu_read(n, n->cpu.PC);
    uint16_t value = read_on_indirect(n, m);
    n->cpu.PC++;
    cpu_tick(n);
    return value + n->cpu.Y;
  }
  case INDIRECT_INDEXED_WITH_PENALTY: {
    uint8_t m = cpu_read(n, n->cpu.PC);
    uint16_t value = read_on_indirect(n, m);
    n->cpu.PC++;
    if (is_page_crossed(n->cpu.Y, value)) {
      cpu_tick(n);
    }
    return value + n->cpu.Y;
  }
  }
  return 0;
}

static uint8_t cpu_pull_stack(nes *n) {
  n->cpu.S++;
  return cpu_read(n, n->cpu.S + 0x0100);
}

static uint16_t cpu_pull_stack_word(nes *n) {
  return (uint16_t)cpu_pull_stack(n) | ((uint16_t)cpu_pull_stack(n) << 8);
}

static void and (nes * n, uint16_t v);
static void eor(nes *n, uint16_t v);
static void ora(nes *n, uint16_t v);
static void abc(nes *n, uint16_t v);
static void sbc(nes *n, uint16_t v);
static void cmp(nes *n, uint8_t m, uint16_t v);
static void asl(nes *n, uint8_t *m);
static void lsr(nes *n, uint8_t *m);
static void rol(nes *n, uint8_t *m);
static void ror(nes *n, uint8_t *m);
static void branch(nes *n, uint8_t v);

static void set_carry_status(nes *n, uint8_t m, uint8_t r);

static void execute(nes *n, cpu_instruction inst) {
  uint16_t operand = get_operand(n, inst.mode);

  switch (inst.mnemonic) {
  case LDA:
    n->cpu.A = cpu_read(n, operand);
    cpu_status_set_zn(&n->cpu, n->cpu.A);
    break;
  case LDX:
    n->cpu.X = cpu_read(n, operand);
    cpu_status_set_zn(&n->cpu, n->cpu.X);
    break;
  case LDY:
    n->cpu.Y = cpu_read(n, operand);
    cpu_status_set_zn(&n->cpu, n->cpu.Y);
    break;
  case STA:
    cpu_write(n, operand, n->cpu.A);
    break;
  case STX:
    cpu_write(n, operand, n->cpu.X);
    break;
  case STY:
    cpu_write(n, operand, n->cpu.Y);
    break;
  case TAX:
    n->cpu.X = n->cpu.A;
    cpu_status_set_zn(&n->cpu, n->cpu.X);
    cpu_tick(n);
    break;
  case TAY:
    n->cpu.Y = n->cpu.A;
    cpu_status_set_zn(&n->cpu, n->cpu.Y);
    cpu_tick(n);
    break;
  case TXA:
    n->cpu.A = n->cpu.X;
    cpu_status_set_zn(&n->cpu, n->cpu.A);
    cpu_tick(n);
    break;
  case TYA:
    n->cpu.A = n->cpu.Y;
    cpu_status_set_zn(&n->cpu, n->cpu.A);
    cpu_tick(n);
    break;
  case TSX:
    n->cpu.X = n->cpu.S;
    cpu_status_set_zn(&n->cpu, n->cpu.X);
    cpu_tick(n);
    break;
  case TXS:
    n->cpu.S = n->cpu.X;
    cpu_tick(n);
    break;

  case PHA:
    push_stack(n, n->cpu.A);
    cpu_tick(n);
    break;
  case PHP:
    push_stack(n, n->cpu.P | cpu_status_instruction_b);
    cpu_tick(n);
    break;
  case PLA:
    n->cpu.A = cpu_pull_stack(n);
    cpu_status_set_zn(&n->cpu, n->cpu.A);
    cpu_tick(n);
    cpu_tick(n);
    break;
  case PLP: {
    n->cpu.P = cpu_pull_stack(n) & ~cpu_status_instruction_b;
    n->cpu.P |= 0b00100000; // for nestest
    cpu_tick(n);
    cpu_tick(n);
    break;
  }

  case AND:
    and(n, operand);
    break;
  case EOR:
    eor(n, operand);
    break;
  case ORA:
    ora(n, operand);
    break;
  case BIT: {
    uint8_t m = cpu_read(n, operand);
    uint8_t b = n->cpu.A & m;
    cpu_status_set(&n->cpu, CPU_STATUS_Z, b == 0);
    cpu_status_set(&n->cpu, CPU_STATUS_V, (m & 0x40) == 0x40);
    cpu_status_set(&n->cpu, CPU_STATUS_N, (m & 0x80) == 0x80);
    break;
  }

  case ADC: {
    uint8_t m = cpu_read(n, operand);
    uint8_t r = n->cpu.A + m;
    if (cpu_status_enabled(&n->cpu, CPU_STATUS_C))
      r++;
    set_carry_status(n, m, r);
    n->cpu.A = r;
    cpu_status_set_zn(&n->cpu, n->cpu.A);
    break;
  }
  case SBC:
    sbc(n, operand);
    break;
  case CMP:
    cmp(n, n->cpu.A, operand);
    break;
  case CPX:
    cmp(n, n->cpu.X, operand);
    break;
  case CPY:
    cmp(n, n->cpu.Y, operand);
    break;

  case INC: {
    uint8_t r = cpu_read(n, operand) + 1;
    cpu_write(n, operand, r);
    cpu_status_set_zn(&n->cpu, r);
    cpu_tick(n);
    break;
  }
  case INX:
    n->cpu.X++;
    cpu_status_set_zn(&n->cpu, n->cpu.X);
    cpu_tick(n);
    break;
  case INY:
    n->cpu.Y++;
    cpu_status_set_zn(&n->cpu, n->cpu.Y);
    cpu_tick(n);
    break;
  case DEC: {
    uint8_t r = cpu_read(n, operand) - 1;
    cpu_write(n, operand, r);
    cpu_status_set_zn(&n->cpu, r);
    cpu_tick(n);
    break;
  }
  case DEX:
    n->cpu.X--;
    cpu_status_set_zn(&n->cpu, n->cpu.X);
    cpu_tick(n);
    break;
  case DEY:
    n->cpu.Y--;
    cpu_status_set_zn(&n->cpu, n->cpu.Y);
    cpu_tick(n);
    break;

  case ASL:
    if (inst.mode == ACCUMULATOR) {
      asl(n, &n->cpu.A);
    } else {
      uint8_t m = cpu_read(n, operand);
      asl(n, &m);
      cpu_write(n, operand, m);
    }
    break;
  case LSR:
    if (inst.mode == ACCUMULATOR) {
      lsr(n, &n->cpu.A);
    } else {
      uint8_t m = cpu_read(n, operand);
      lsr(n, &m);
      cpu_write(n, operand, m);
    }
    break;
  case ROL:
    if (inst.mode == ACCUMULATOR) {
      rol(n, &n->cpu.A);
    } else {
      uint8_t m = cpu_read(n, operand);
      rol(n, &m);
      cpu_write(n, operand, m);
    }
    break;
  case ROR:
    if (inst.mode == ACCUMULATOR) {
      ror(n, &n->cpu.A);
    } else {
      uint8_t m = cpu_read(n, operand);
      ror(n, &m);
      cpu_write(n, operand, m);
    }
    break;

  case JMP:
    n->cpu.PC = operand;
    break;
  case JSR:
    push_stack_word(n, n->cpu.PC - 1);
    n->cpu.PC = operand;
    cpu_tick(n);
    break;
  case RTS:
    n->cpu.PC = cpu_pull_stack_word(n);
    n->cpu.PC++;
    cpu_tick(n);
    cpu_tick(n);
    cpu_tick(n);
    break;

  case BCC:
    if (!cpu_status_enabled(&n->cpu, CPU_STATUS_C))
      branch(n, operand);
    break;
  case BCS:
    if (cpu_status_enabled(&n->cpu, CPU_STATUS_C))
      branch(n, operand);
    break;
  case BEQ:
    if (cpu_status_enabled(&n->cpu, CPU_STATUS_Z))
      branch(n, operand);
    break;
  case BMI:
    if (cpu_status_enabled(&n->cpu, CPU_STATUS_N))
      branch(n, operand);
    break;
  case BNE:
    if (!cpu_status_enabled(&n->cpu, CPU_STATUS_Z))
      branch(n, operand);
    break;
  case BPL:
    if (!cpu_status_enabled(&n->cpu, CPU_STATUS_N))
      branch(n, operand);
    break;
  case BVC:
    if (!cpu_status_enabled(&n->cpu, CPU_STATUS_V))
      branch(n, operand);
    break;
  case BVS:
    if (cpu_status_enabled(&n->cpu, CPU_STATUS_V))
      branch(n, operand);
    break;

  case CLC:
    cpu_status_set(&n->cpu, CPU_STATUS_C, false);
    cpu_tick(n);
    break;
  case CLD:
    cpu_status_set(&n->cpu, CPU_STATUS_D, false);
    cpu_tick(n);
    break;
  case CLI:
    cpu_status_set(&n->cpu, CPU_STATUS_I, false);
    cpu_tick(n);
    break;
  case CLV:
    cpu_status_set(&n->cpu, CPU_STATUS_V, false);
    cpu_tick(n);
    break;
  case SEC:
    cpu_status_set(&n->cpu, CPU_STATUS_C, true);
    cpu_tick(n);
    break;
  case SED:
    cpu_status_set(&n->cpu, CPU_STATUS_D, true);
    cpu_tick(n);
    break;
  case SEI:
    cpu_status_set(&n->cpu, CPU_STATUS_I, true);
    cpu_tick(n);
    break;

  case BRK:
    push_stack_word(n, n->cpu.PC);
    push_stack(n, n->cpu.P);
    n->cpu.P |= cpu_status_instruction_b;
    n->cpu.PC = cpu_read_word(n, 0xFFFE);
    cpu_tick(n);
    break;
  case NOP:
    cpu_tick(n);
    break;
  case RTI:
    n->cpu.P = cpu_pull_stack(n);
    n->cpu.P = n->cpu.P & ~cpu_status_instruction_b;
    n->cpu.PC = cpu_pull_stack_word(n);
    cpu_tick(n);
    cpu_tick(n);
    break;

  case LAX: {
    uint8_t m = cpu_read(n, operand);
    n->cpu.A = m;
    cpu_status_set_zn(&n->cpu, m);
    n->cpu.X = m;
    break;
  }
  case SAX:
    cpu_write(n, operand, n->cpu.A & n->cpu.X);
    break;
  case DCP: {
    // decrementMemory excluding tick
    uint8_t m = cpu_read(n, operand) - 1;
    cpu_status_set_zn(&n->cpu, m);
    cpu_write(n, operand, m);
    cmp(n, n->cpu.A, operand);
    break;
  }
  case ISB: {
    // incrementMemory excluding tick
    uint8_t m = cpu_read(n, operand) + 1;
    cpu_status_set_zn(&n->cpu, m);
    cpu_write(n, operand, m);
    sbc(n, operand);
    break;
  }
  case SLO: {
    // arithmeticShiftLeft excluding tick
    uint8_t m = cpu_read(n, operand);
    cpu_status_set(&n->cpu, CPU_STATUS_C, (m & 0x80) == 0x80);
    m <<= 1;
    cpu_write(n, operand, m);
    ora(n, operand);
    break;
  }
  case RLA: {
    // rotateLeft excluding tick
    uint8_t m = cpu_read(n, operand);
    uint8_t carry = m & 0x80;
    m <<= 1;
    if (cpu_status_enabled(&n->cpu, CPU_STATUS_C)) {
      m |= 1;
    }
    cpu_status_set(&n->cpu, CPU_STATUS_C, carry == 0x80);
    cpu_status_set_zn(&n->cpu, m);
    cpu_write(n, operand, m);
    and(n, operand);
    break;
  }
  case SRE: {
    // logicalShiftRight excluding tick
    uint8_t m = cpu_read(n, operand);
    cpu_status_set(&n->cpu, CPU_STATUS_C, (m & 1) == 1);
    m >>= 1;
    cpu_status_set_zn(&n->cpu, m);
    cpu_write(n, operand, m);
    eor(n, operand);
    break;
  }
  case RRA: {
    // rotateRight excluding tick
    uint8_t m = cpu_read(n, operand);
    uint8_t carry = m & 1;
    m >>= 1;
    if (cpu_status_enabled(&n->cpu, CPU_STATUS_C)) {
      m |= 0x80;
    }
    cpu_status_set(&n->cpu, CPU_STATUS_C, carry == 1);
    cpu_status_set_zn(&n->cpu, m);
    cpu_write(n, operand, m);
    abc(n, operand);
    break;
  }
  default: // NOP
    cpu_tick(n);
    break;
  }
}

void cpu_step(nes *n) {
  handle_interrupt(n);

  // fetch
  uint8_t op = cpu_read(n, n->cpu.PC);
  n->cpu.PC++;

  cpu_instruction inst = cpu_decode(op);
  execute(n, inst);
}

void and (nes * n, uint16_t v) {
  n->cpu.A &= cpu_read(n, v);
  cpu_status_set_zn(&n->cpu, n->cpu.A);
}

void eor(nes *n, uint16_t v) {
  n->cpu.A ^= cpu_read(n, v);
  cpu_status_set_zn(&n->cpu, n->cpu.A);
}

void ora(nes *n, uint16_t v) {
  n->cpu.A |= cpu_read(n, v);
  cpu_status_set_zn(&n->cpu, n->cpu.A);
}

void abc(nes *n, uint16_t v) {
  uint8_t m = cpu_read(n, v);
  uint8_t r = n->cpu.A + m;
  if (cpu_status_enabled(&n->cpu, CPU_STATUS_C)) {
    r++;
  }
  set_carry_status(n, m, r);
  n->cpu.A = r;
  cpu_status_set_zn(&n->cpu, n->cpu.A);
}

void sbc(nes *n, uint16_t v) {
  uint8_t m = ~cpu_read(n, v);
  uint8_t r = n->cpu.A + m;
  if (cpu_status_enabled(&n->cpu, CPU_STATUS_C)) {
    r++;
  }
  set_carry_status(n, m, r);
  n->cpu.A = r;
  cpu_status_set_zn(&n->cpu, n->cpu.A);
}

void cmp(nes *n, uint8_t m, uint16_t v) {
  int16_t r = (int16_t)m - (int16_t)cpu_read(n, v);
  cpu_status_set_zn(&n->cpu, r);
  cpu_status_set(&n->cpu, CPU_STATUS_C, 0 <= r);
}

void asl(nes *n, uint8_t *m) {
  cpu_status_set(&n->cpu, CPU_STATUS_C, (*m & 0x80) == 0x80);
  *m <<= 1;
  cpu_status_set_zn(&n->cpu, *m);
  cpu_tick(n);
}

void lsr(nes *n, uint8_t *m) {
  cpu_status_set(&n->cpu, CPU_STATUS_C, (*m & 1) == 1);
  *m >>= 1;
  cpu_status_set_zn(&n->cpu, *m);
  cpu_tick(n);
}

void rol(nes *n, uint8_t *m) {
  uint8_t carry = *m & 0x80;
  *m <<= 1;
  if (cpu_status_enabled(&n->cpu, CPU_STATUS_C)) {
    *m |= 1;
  }
  cpu_status_set(&n->cpu, CPU_STATUS_C, carry == 0x80);
  cpu_status_set_zn(&n->cpu, *m);
  cpu_tick(n);
}

void ror(nes *n, uint8_t *m) {
  uint8_t carry = *m & 1;
  *m >>= 1;
  if (cpu_status_enabled(&n->cpu, CPU_STATUS_C)) {
    *m |= 0x80;
  }
  cpu_status_set(&n->cpu, CPU_STATUS_C, carry == 1);
  cpu_status_set_zn(&n->cpu, *m);
  cpu_tick(n);
}

void branch(nes *n, uint8_t v) {
  cpu_tick(n);
  int16_t base = (int16_t)n->cpu.PC;
  int8_t offset = (int8_t)v; // to negative number
  if (is_page_crossed(offset, base)) {
    cpu_tick(n);
  }
  n->cpu.PC = base + (int16_t)offset;
}

void set_carry_status(nes *n, uint8_t m, uint8_t r) {
  uint8_t a7 = n->cpu.A >> 7 & 1;
  uint8_t m7 = m >> 7 & 1;
  uint8_t c6 = a7 ^ m7 ^ (r >> 7 & 1);
  uint8_t c7 = (a7 & m7) | (a7 & c6) | (m7 & c6);
  cpu_status_set(&n->cpu, CPU_STATUS_C, c7 == 1);
  cpu_status_set(&n->cpu, CPU_STATUS_V, (c6 ^ c7) == 1);
}
