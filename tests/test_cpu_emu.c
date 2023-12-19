#include "test_common.h"

#include "cpu_decoder.h"
#include "cpu_emu.h"
#include "nes.h"
#include "nes_emu.h"

#include "mapper.h"

#include "mem.h"
#include "mock_mem.h"

uint16_t cpu_get_operand(NES *nes, AddressingMode mode);

Mapper *mock_mapper_new(uint8_t mapper_no, MirroringMode mirroring);

NES *nes_init() {
  NES *nes = nes_new();
  Mapper *mapper = mock_mapper_new(0, MIRRORING_HORIZONTAL);
  nes_insert_cartridge(nes, mapper);
  return nes;
}

TEST(test_get_operand_implicit) {
  NES *nes = nes_init();

  uint16_t result = cpu_get_operand(nes, IMPLICIT);
  test_assert_int_eq(0, result);
  test_assert_int_eq(0, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_get_operand_accumulator) {
  NES *nes = nes_init();
  nes->cpu.A = 0xFB;

  uint16_t result = cpu_get_operand(nes, ACCUMULATOR);
  test_assert_byte_eq(0xFB, result);
  test_assert_int_eq(0, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_get_operand_immediate) {
  NES *nes = nes_init();
  nes->cpu.PC = 0x8234;

  uint16_t result = cpu_get_operand(nes, IMMEDIATE);
  test_assert_byte_eq(0x8234, result);
  test_assert_int_eq(0, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_get_operand_zero_page) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x0414;
  mem_write(nes, 0x0414, 0x91);

  uint16_t result = cpu_get_operand(nes, ZERO_PAGE);
  test_assert_byte_eq(0x91, result);
  test_assert_int_eq(1, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_get_operand_zero_page_x) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x0100;
  nes->cpu.X = 0x93;
  mem_write(nes, 0x0100, 0x80);

  uint16_t result = cpu_get_operand(nes, ZERO_PAGE_X);
  test_assert_byte_eq(0x13, result);
  test_assert_int_eq(2, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_get_operand_zero_page_y) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x0423;
  nes->cpu.Y = 0xF1;
  mem_write(nes, 0x0423, 0x36);

  uint16_t result = cpu_get_operand(nes, ZERO_PAGE_Y);
  test_assert_byte_eq(0x27, result);
  test_assert_int_eq(2, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_get_operand_absolute) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x0423;
  mem_write(nes, 0x0423, 0x36);
  mem_write(nes, 0x0424, 0xF0);

  uint16_t result = cpu_get_operand(nes, ABSOLUTE);
  test_assert_byte_eq(0xF036, result);
  test_assert_int_eq(2, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_get_operand_absolute_x) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x0423;
  mem_write(nes, 0x0423, 0x36);
  mem_write(nes, 0x0424, 0xF0);

  nes->cpu.X = 0x31;

  uint16_t result = cpu_get_operand(nes, ABSOLUTE_X);
  test_assert_byte_eq(0xF067, result);
  test_assert_int_eq(3, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_get_operand_absolute_x_with_penalty_not_page_crossed) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x0423;
  mem_write(nes, 0x0423, 0x36);
  mem_write(nes, 0x0424, 0xF0);

  nes->cpu.X = 0x31;

  uint16_t result = cpu_get_operand(nes, ABSOLUTE_X_WITH_PENALTY);
  test_assert_byte_eq(0xF067, result);
  test_assert_int_eq(2, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_get_operand_absolute_x_with_penalty_page_crossed) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x0423;
  mem_write(nes, 0x0423, 0x36);
  mem_write(nes, 0x0424, 0xF0);

  nes->cpu.X = 0xF0;

  uint16_t result = cpu_get_operand(nes, ABSOLUTE_X_WITH_PENALTY);
  test_assert_byte_eq(0xF126, result);
  test_assert_int_eq(3, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_get_operand_absolute_y) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x0423;
  mem_write(nes, 0x0423, 0x36);
  mem_write(nes, 0x0424, 0xF0);

  nes->cpu.Y = 0x31;

  uint16_t result = cpu_get_operand(nes, ABSOLUTE_Y);
  test_assert_byte_eq(0xF067, result);
  test_assert_int_eq(3, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_get_operand_absolute_y_with_penalty_not_page_crossed) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x0423;
  mem_write(nes, 0x0423, 0x36);
  mem_write(nes, 0x0424, 0xF0);

  nes->cpu.Y = 0x31;

  uint16_t result = cpu_get_operand(nes, ABSOLUTE_Y_WITH_PENALTY);
  test_assert_byte_eq(0xF067, result);
  test_assert_int_eq(2, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_get_operand_absolute_y_with_penalty_page_crossed) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x0423;
  mem_write(nes, 0x0423, 0x36);
  mem_write(nes, 0x0424, 0xF0);

  nes->cpu.Y = 0xF0;

  uint16_t result = cpu_get_operand(nes, ABSOLUTE_Y_WITH_PENALTY);
  test_assert_byte_eq(0xF126, result);
  test_assert_int_eq(3, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_get_operand_relative) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x0414;
  mem_write(nes, 0x0414, 0x91);

  uint16_t result = cpu_get_operand(nes, RELATIVE);
  test_assert_byte_eq(0x91, result);
  test_assert_int_eq(1, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_get_operand_indirect) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  mem_write(nes, 0x020F, 0x10);
  mem_write(nes, 0x0210, 0x03);
  mem_write(nes, 0x0310, 0x9F);

  uint16_t result = cpu_get_operand(nes, INDIRECT);
  test_assert_byte_eq(0x9F, result);
  test_assert_int_eq(4, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_get_operand_indexed_indirect) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.X = 0x95;
  mem_write(nes, 0x020F, 0xF0);
  mem_write(nes, 0x0085, 0x12);
  mem_write(nes, 0x0086, 0x90);

  uint16_t result = cpu_get_operand(nes, INDEXED_INDIRECT);
  test_assert_byte_eq(0x9012, result);
  test_assert_int_eq(4, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_get_operand_indirect_indexed) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  mem_write(nes, 0x020F, 0xF0);
  mem_write(nes, 0x00F0, 0x12);
  mem_write(nes, 0x00F1, 0x90);

  nes->cpu.Y = 0xF3;

  uint16_t result = cpu_get_operand(nes, INDIRECT_INDEXED);
  test_assert_byte_eq(0x9105, result);
  test_assert_int_eq(4, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_get_operand_indirect_indexed_with_penalty_not_page_crossed) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  mem_write(nes, 0x020F, 0xF0);
  mem_write(nes, 0x00F0, 0x12);
  mem_write(nes, 0x00F1, 0x90);

  nes->cpu.Y = 0x83;

  uint16_t result = cpu_get_operand(nes, INDIRECT_INDEXED_WITH_PENALTY);
  test_assert_byte_eq(0x9095, result);
  test_assert_int_eq(3, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_get_operand_indirect_indexed_with_penalty_page_crossed) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  mem_write(nes, 0x020F, 0xF0);
  mem_write(nes, 0x00F0, 0x12);
  mem_write(nes, 0x00F1, 0x90);

  nes->cpu.Y = 0xF3;

  uint16_t result = cpu_get_operand(nes, INDIRECT_INDEXED_WITH_PENALTY);
  test_assert_byte_eq(0x9105, result);
  test_assert_int_eq(4, nes->cpu.cycles);

  nes_release(nes);
}

TEST_SUITE(test_get_operand) {
  RUN_TEST(test_get_operand_implicit);
  RUN_TEST(test_get_operand_accumulator);
  RUN_TEST(test_get_operand_zero_page);
  RUN_TEST(test_get_operand_zero_page_x);
  RUN_TEST(test_get_operand_zero_page_y);
  RUN_TEST(test_get_operand_absolute);
  RUN_TEST(test_get_operand_absolute_x);
  RUN_TEST(test_get_operand_absolute_x_with_penalty_not_page_crossed);
  RUN_TEST(test_get_operand_absolute_x_with_penalty_page_crossed);
  RUN_TEST(test_get_operand_absolute_y);
  RUN_TEST(test_get_operand_absolute_y_with_penalty_not_page_crossed);
  RUN_TEST(test_get_operand_absolute_y_with_penalty_page_crossed);
  RUN_TEST(test_get_operand_relative);
  RUN_TEST(test_get_operand_indirect);
  RUN_TEST(test_get_operand_indexed_indirect);
  RUN_TEST(test_get_operand_indirect_indexed);
  RUN_TEST(test_get_operand_indirect_indexed_with_penalty_not_page_crossed);
  RUN_TEST(test_get_operand_indirect_indexed_with_penalty_page_crossed);
}

TEST(test_LDA) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  mem_write(nes, 0x020F, 0xA9);
  mem_write(nes, 0x0210, 0x31);

  cpu_step(nes);

  test_assert_byte_eq(0x31, nes->cpu.A);
  test_assert_int_eq(2, nes->cpu.cycles);
  test_assert_byte_eq(0, nes->cpu.P);

  nes_release(nes);
}

TEST(test_STA) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.A = 0x91;
  mem_write(nes, 0x020F, 0x8D);
  mem_write(nes, 0x0210, 0x19);
  mem_write(nes, 0x0211, 0x04);

  cpu_step(nes);

  test_assert_byte_eq(0x91, mem_read(nes, 0x0419));
  test_assert_int_eq(4, nes->cpu.cycles);
  test_assert_byte_eq(0, nes->cpu.P);

  nes_release(nes);
}

TEST(test_TAX) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.A = 0x83;
  mem_write(nes, 0x020F, 0xAA);

  cpu_step(nes);

  test_assert_byte_eq(0x83, nes->cpu.X);
  test_assert_int_eq(2, nes->cpu.cycles);
  test_assert_byte_eq(0x80, nes->cpu.P);

  nes_release(nes);
}

TEST(test_TYA) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.Y = 0xF0;
  mem_write(nes, 0x020F, 0x98);

  cpu_step(nes);

  test_assert_byte_eq(0xF0, nes->cpu.A);
  test_assert_int_eq(2, nes->cpu.cycles);
  test_assert_byte_eq(0x80, nes->cpu.P);

  nes_release(nes);
}

TEST(test_TSX) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.S = 0xF3;
  mem_write(nes, 0x020F, 0xBA);

  cpu_step(nes);

  test_assert_byte_eq(0xF3, nes->cpu.X);
  test_assert_int_eq(2, nes->cpu.cycles);
  test_assert_byte_eq(0x80, nes->cpu.P);

  nes_release(nes);
}

TEST(test_PHA) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.S = 0xFD;
  nes->cpu.A = 0x72;
  mem_write(nes, 0x020F, 0x48);

  cpu_step(nes);

  test_assert_byte_eq(0xFC, nes->cpu.S);
  test_assert_byte_eq(0x72, mem_read(nes, 0x01FD));
  test_assert_int_eq(3, nes->cpu.cycles);
  test_assert_byte_eq(0, nes->cpu.P);

  nes_release(nes);
}

TEST(test_PHP) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.S = 0xFD;
  nes->cpu.A = 0x72;
  mem_write(nes, 0x020F, 0x08);
  nes->cpu.P = 0b10001001;

  cpu_step(nes);

  test_assert_byte_eq(0xFC, nes->cpu.S);
  test_assert_bit_eq(nes->cpu.P | 0b00110000, mem_read(nes, 0x01FD));
  test_assert_int_eq(3, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_PLP) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.S = 0xBF;
  mem_write(nes, 0x020F, 0x28);
  mem_write(nes, 0x01C0, 0x7A);

  cpu_step(nes);

  test_assert_byte_eq(0xC0, nes->cpu.S);
  test_assert_bit_eq(0b01101010, nes->cpu.P);
  test_assert_int_eq(4, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_EOR) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.A = 0x21;
  mem_write(nes, 0x020F, 0x49);
  mem_write(nes, 0x0210, 0x38);

  cpu_step(nes);

  test_assert_byte_eq(0x19, nes->cpu.A);
  test_assert_bit_eq(0, nes->cpu.P);
  test_assert_int_eq(2, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_BIT) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.A = 0x48;
  mem_write(nes, 0x020F, 0x2C);
  mem_write(nes, 0x0210, 0xB0);
  mem_write(nes, 0x0211, 0x03);
  mem_write(nes, 0x03B0, 0b11000000);

  cpu_step(nes);

  test_assert_bit_eq(0b11000000, nes->cpu.P);
  test_assert_int_eq(4, nes->cpu.cycles);

  nes_release(nes);
}

struct adc_test_pattern {
  uint8_t input_a, input_m;
  uint8_t expected_a, expected_p;
};

TEST(test_ADC) {
  struct adc_test_pattern test_cases[8] = {
      {0x50, 0x10, 0x60, 0b00000000}, {0x50, 0x50, 0xA0, 0b11000000},
      {0x50, 0x90, 0xE0, 0b10000000}, {0x50, 0xD0, 0x20, 0b00000001},
      {0xD0, 0x10, 0xE0, 0b10000000}, {0xD0, 0x50, 0x20, 0b00000001},
      {0xD0, 0x90, 0x60, 0b01000001}, {0xD0, 0xD0, 0xA0, 0b10000001},
  };
  for (int i = 0; i < 8; i++) {
    struct adc_test_pattern test_case = test_cases[i];
    mem_init();

    NES *nes = nes_init();
    nes->cpu.PC = 0x020F;
    nes->cpu.A = test_case.input_a;

    mem_write(nes, 0x020F, 0x6D);
    mem_write(nes, 0x0210, 0xD3);
    mem_write(nes, 0x0211, 0x04);
    mem_write(nes, 0x04D3, test_case.input_m);

    cpu_step(nes);

    test_assert_byte_eq(test_case.expected_a, nes->cpu.A);
    test_assert_bit_eq(test_case.expected_p, nes->cpu.P);

    nes_release(nes);
  }
}

TEST(test_CPY) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.Y = 0x37;
  mem_write(nes, 0x020F, 0xCC);
  mem_write(nes, 0x0210, 0x36);

  cpu_step(nes);

  test_assert_bit_eq(0b00000001, nes->cpu.P);
  test_assert_int_eq(4, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_INC) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  mem_write(nes, 0x020F, 0xEE);
  mem_write(nes, 0x0210, 0xD3);
  mem_write(nes, 0x0211, 0x04);
  mem_write(nes, 0x04D3, 0x7F);

  cpu_step(nes);

  test_assert_bit_eq(0b10000000, nes->cpu.P);
  test_assert_byte_eq(0x80, mem_read(nes, 0x04D3));
  test_assert_int_eq(6, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_DEC) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  mem_write(nes, 0x020F, 0xCE);
  mem_write(nes, 0x0210, 0xD3);
  mem_write(nes, 0x0211, 0x04);
  mem_write(nes, 0x04D3, 0xC0);

  cpu_step(nes);

  test_assert_bit_eq(0b10000000, nes->cpu.P);
  test_assert_byte_eq(0xBF, mem_read(nes, 0x04D3));
  test_assert_int_eq(6, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_ASL) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.A = 0b10001010;
  mem_write(nes, 0x020F, 0x0A);

  cpu_step(nes);

  test_assert_bit_eq(0b00010100, nes->cpu.A);
  test_assert_bit_eq(0b00000001, nes->cpu.P);
  test_assert_int_eq(2, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_ROL) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.A = 0b10001010;
  nes->cpu.P = 0b00000001;
  mem_write(nes, 0x020F, 0x2A);

  cpu_step(nes);

  test_assert_bit_eq(0b00010101, nes->cpu.A);
  test_assert_bit_eq(0b00000001, nes->cpu.P);
  test_assert_int_eq(2, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_ROL_carry) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.A = 0b10001010;
  nes->cpu.P = 0b10000000;
  mem_write(nes, 0x020F, 0x2A);

  cpu_step(nes);

  test_assert_bit_eq(0b00010100, nes->cpu.A);
  test_assert_bit_eq(0b00000001, nes->cpu.P);
  test_assert_int_eq(2, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_JSR) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.S = 0xBF;
  mem_write(nes, 0x020F, 0x20);
  mem_write(nes, 0x0210, 0x31);
  mem_write(nes, 0x0211, 0x40);

  cpu_step(nes);

  test_assert_byte_eq(0xBD, nes->cpu.S);
  test_assert_byte_eq(0x4031, nes->cpu.PC);
  test_assert_byte_eq(0x11, mem_read(nes, 0x01BE));
  test_assert_byte_eq(0x02, mem_read(nes, 0x01BF));
  test_assert_int_eq(6, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_RTS) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x0031;
  nes->cpu.S = 0xBD;
  mem_write(nes, 0x0031, 0x60);
  mem_write(nes, 0x01BE, 0x11);
  mem_write(nes, 0x01BF, 0x02);

  cpu_step(nes);

  test_assert_byte_eq(0xBF, nes->cpu.S);
  test_assert_byte_eq(0x0212, nes->cpu.PC);
  test_assert_int_eq(6, nes->cpu.cycles);

  nes_release(nes);
}

struct bcc_test_pattern {
  uint8_t input_operand, input_p;
  uint8_t expected_pc, expected_cycles;
};

TEST(test_BCC) {
  struct bcc_test_pattern test_cases[3] = {
      {0x03, 0b10000001, 0x33, 2}, // branch failed
      {0x03, 0b11000000, 0x36, 3}, // branch succeed
      {0xD0, 0b11000000, 0x03, 3}, // branch succeed & new page
  };
  for (int i = 0; i < 1; i++) {
    struct bcc_test_pattern test_case = test_cases[i];
    mem_init();

    NES *nes = nes_init();
    nes->cpu.PC = 0x0031;
    nes->cpu.P = test_case.input_p;

    mem_write(nes, 0x0031, 0x90);
    mem_write(nes, 0x0032, test_case.input_operand);

    cpu_step(nes);

    test_assert_byte_eq(test_case.expected_pc, nes->cpu.PC);
    test_assert_int_eq(test_case.expected_cycles, nes->cpu.cycles);

    nes_release(nes);
  }
}

TEST(test_CLD) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.P = 0b011001001;
  mem_write(nes, 0x020F, 0xD8);

  cpu_step(nes);

  test_assert_byte_eq(0x0210, nes->cpu.PC);
  test_assert_bit_eq(0b011000001, nes->cpu.P);
  test_assert_int_eq(2, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_SEI) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.P = 0b011001001;
  mem_write(nes, 0x020F, 0x78);

  cpu_step(nes);

  test_assert_byte_eq(0x0210, nes->cpu.PC);
  test_assert_bit_eq(0b011001101, nes->cpu.P);
  test_assert_int_eq(2, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_BRK) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.P = 0b01100001;
  nes->cpu.S = 0xBF;
  mem_write(nes, 0x020F, 0x00);
  mem_write(nes, 0xFFFE, 0x23);
  mem_write(nes, 0xFFFF, 0x40);

  cpu_step(nes);

  test_assert_byte_eq(0x4023, nes->cpu.PC);
  test_assert_bit_eq(0b01110001, nes->cpu.P);
  test_assert_byte_eq(0xBC, nes->cpu.S);
  test_assert_int_eq(7, nes->cpu.cycles);

  nes_release(nes);
}

TEST(test_RTI) {
  mem_init();

  NES *nes = nes_init();
  nes->cpu.PC = 0x020F;
  nes->cpu.P = 0b01100101;
  nes->cpu.S = 0xBC;
  mem_write(nes, 0x020F, 0x40);
  mem_write(nes, 0x01BD, 0b10000010);
  mem_write(nes, 0x01BE, 0x11);
  mem_write(nes, 0x01BF, 0x02);

  cpu_step(nes);

  test_assert_byte_eq(0x0211, nes->cpu.PC);
  test_assert_bit_eq(0b10000010, nes->cpu.P);
  test_assert_byte_eq(0xBF, nes->cpu.S);
  test_assert_int_eq(6, nes->cpu.cycles);

  nes_release(nes);
}

TEST_SUITE(test_execute) {
  RUN_TEST(test_LDA);
  RUN_TEST(test_STA);
  RUN_TEST(test_TAX);
  RUN_TEST(test_TYA);
  RUN_TEST(test_TSX);
  RUN_TEST(test_PHA);
  RUN_TEST(test_PHP);
  RUN_TEST(test_PLP);
  RUN_TEST(test_EOR);
  RUN_TEST(test_BIT);
  RUN_TEST(test_ADC);
  RUN_TEST(test_CPY);
  RUN_TEST(test_INC);
  RUN_TEST(test_DEC);
  RUN_TEST(test_ASL);
  RUN_TEST(test_ROL);
  RUN_TEST(test_ROL_carry);
  RUN_TEST(test_JSR);
  RUN_TEST(test_RTS);
  RUN_TEST(test_BCC);
  RUN_TEST(test_CLD);
  RUN_TEST(test_SEI);
  RUN_TEST(test_BRK);
  RUN_TEST(test_RTI);
}

int main(int argc, char *argv[]) {
  RUN_TEST_SUITE(test_get_operand);
  RUN_TEST_SUITE(test_execute);
  TEST_REPORT();
  return TEST_EXIT_CODE;
}
