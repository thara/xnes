#include "test_common.h"

#include "cpu_instruction.h"
#include "cpu_emulation.h"
#include "nes.h"

#include "mem.h"
#include "mock_mem.h"

uint16_t cpu_get_operand(struct nes *nes, enum cpu_addressing_mode mode);

TEST(test_get_operand_implicit) {
    struct nes* nes = nes_new();

    uint16_t result = cpu_get_operand(nes, implicit);
    test_assert_int_eq(0, result);
    test_assert_int_eq(0, nes->cpu->cycles);
}

TEST(test_get_operand_accumulator) {
    struct nes* nes = nes_new();
    nes->cpu->A = 0xFB;

    uint16_t result = cpu_get_operand(nes, accumulator);
    test_assert_byte_eq(0xFB, result);
    test_assert_int_eq(0, nes->cpu->cycles);
}

TEST(test_get_operand_immediate) {
    struct nes* nes = nes_new();
    nes->cpu->PC = 0x8234;

    uint16_t result = cpu_get_operand(nes, immediate);
    test_assert_byte_eq(0x8234, result);
    test_assert_int_eq(0, nes->cpu->cycles);
}

TEST(test_get_operand_zero_page) {
    mem_init();

    struct nes* nes = nes_new();
    nes->cpu->PC = 0x0414;
    mem_write(nes, 0x0414, 0x91);

    uint16_t result = cpu_get_operand(nes, zero_page);
    test_assert_byte_eq(0x91, result);
    test_assert_int_eq(1, nes->cpu->cycles);
}

TEST(test_get_operand_zero_page_x) {
    mem_init();

    struct nes* nes = nes_new();
    nes->cpu->PC = 0x0100;
    nes->cpu->X = 0x93;
    mem_write(nes, 0x0100, 0x80);

    uint16_t result = cpu_get_operand(nes, zero_page_x);
    test_assert_byte_eq(0x13, result);
    test_assert_int_eq(2, nes->cpu->cycles);
}

TEST(test_get_operand_zero_page_y) {
    mem_init();

    struct nes* nes = nes_new();
    nes->cpu->PC = 0x0423;
    nes->cpu->Y = 0xF1;
    mem_write(nes, 0x0423, 0x36);

    uint16_t result = cpu_get_operand(nes, zero_page_y);
    test_assert_byte_eq(0x27, result);
    test_assert_int_eq(2, nes->cpu->cycles);
}

TEST(test_get_operand_absolute) {
    mem_init();

    struct nes* nes = nes_new();
    nes->cpu->PC = 0x0423;
    mem_write(nes, 0x0423, 0x36);
    mem_write(nes, 0x0424, 0xF0);

    uint16_t result = cpu_get_operand(nes, absolute);
    test_assert_byte_eq(0xF036, result);
    test_assert_int_eq(2, nes->cpu->cycles);
}

TEST(test_get_operand_absolute_x) {
    mem_init();

    struct nes* nes = nes_new();
    nes->cpu->PC = 0x0423;
    mem_write(nes, 0x0423, 0x36);
    mem_write(nes, 0x0424, 0xF0);

    nes->cpu->X = 0x31;

    uint16_t result = cpu_get_operand(nes, absolute_x);
    test_assert_byte_eq(0xF067, result);
    test_assert_int_eq(3, nes->cpu->cycles);
}

TEST(test_get_operand_absolute_x_with_penalty_not_page_crossed) {
    mem_init();

    struct nes* nes = nes_new();
    nes->cpu->PC = 0x0423;
    mem_write(nes, 0x0423, 0x36);
    mem_write(nes, 0x0424, 0xF0);

    nes->cpu->X = 0x31;

    uint16_t result = cpu_get_operand(nes, absolute_x_with_penalty);
    test_assert_byte_eq(0xF067, result);
    test_assert_int_eq(2, nes->cpu->cycles);
}

TEST(test_get_operand_absolute_x_with_penalty_page_crossed) {
    mem_init();

    struct nes* nes = nes_new();
    nes->cpu->PC = 0x0423;
    mem_write(nes, 0x0423, 0x36);
    mem_write(nes, 0x0424, 0xF0);

    nes->cpu->X = 0xF0;

    uint16_t result = cpu_get_operand(nes, absolute_x_with_penalty);
    test_assert_byte_eq(0xF126, result);
    test_assert_int_eq(3, nes->cpu->cycles);
}

TEST(test_get_operand_absolute_y) {
    mem_init();

    struct nes* nes = nes_new();
    nes->cpu->PC = 0x0423;
    mem_write(nes, 0x0423, 0x36);
    mem_write(nes, 0x0424, 0xF0);

    nes->cpu->Y = 0x31;

    uint16_t result = cpu_get_operand(nes, absolute_y);
    test_assert_byte_eq(0xF067, result);
    test_assert_int_eq(3, nes->cpu->cycles);
}

TEST(test_get_operand_absolute_y_with_penalty_not_page_crossed) {
    mem_init();

    struct nes* nes = nes_new();
    nes->cpu->PC = 0x0423;
    mem_write(nes, 0x0423, 0x36);
    mem_write(nes, 0x0424, 0xF0);

    nes->cpu->Y = 0x31;

    uint16_t result = cpu_get_operand(nes, absolute_y_with_penalty);
    test_assert_byte_eq(0xF067, result);
    test_assert_int_eq(2, nes->cpu->cycles);
}

TEST(test_get_operand_absolute_y_with_penalty_page_crossed) {
    mem_init();

    struct nes* nes = nes_new();
    nes->cpu->PC = 0x0423;
    mem_write(nes, 0x0423, 0x36);
    mem_write(nes, 0x0424, 0xF0);

    nes->cpu->Y = 0xF0;

    uint16_t result = cpu_get_operand(nes, absolute_y_with_penalty);
    test_assert_byte_eq(0xF126, result);
    test_assert_int_eq(3, nes->cpu->cycles);
}

TEST(test_get_operand_relative) {
    mem_init();

    struct nes* nes = nes_new();
    nes->cpu->PC = 0x0414;
    mem_write(nes, 0x0414, 0x91);

    uint16_t result = cpu_get_operand(nes, relative);
    test_assert_byte_eq(0x91, result);
    test_assert_int_eq(1, nes->cpu->cycles);
}

TEST(test_get_operand_indirect) {
    mem_init();

    struct nes* nes = nes_new();
    nes->cpu->PC = 0x020F;
    mem_write(nes, 0x020F, 0x10);
    mem_write(nes, 0x0210, 0x03);
    mem_write(nes, 0x0310, 0x9F);

    uint16_t result = cpu_get_operand(nes, indirect);
    test_assert_byte_eq(0x9F, result);
    test_assert_int_eq(4, nes->cpu->cycles);
}

TEST(test_get_operand_indexed_indirect) {
    mem_init();

    struct nes* nes = nes_new();
    nes->cpu->PC = 0x020F;
    nes->cpu->X = 0x95;
    mem_write(nes, 0x020F, 0xF0);
    mem_write(nes, 0x0085, 0x12);
    mem_write(nes, 0x0086, 0x90);

    uint16_t result = cpu_get_operand(nes, indexed_indirect);
    test_assert_byte_eq(0x9012, result);
    test_assert_int_eq(4, nes->cpu->cycles);
}

TEST(test_get_operand_indirect_indexed) {
    mem_init();

    struct nes* nes = nes_new();
    nes->cpu->PC = 0x020F;
    mem_write(nes, 0x020F, 0xF0);
    mem_write(nes, 0x00F0, 0x12);
    mem_write(nes, 0x00F1, 0x90);

    nes->cpu->Y = 0xF3;

    uint16_t result = cpu_get_operand(nes, indirect_indexed);
    test_assert_byte_eq(0x9105, result);
    test_assert_int_eq(4, nes->cpu->cycles);
}

TEST(test_get_operand_indirect_indexed_with_penalty_not_page_crossed) {
    mem_init();

    struct nes* nes = nes_new();
    nes->cpu->PC = 0x020F;
    mem_write(nes, 0x020F, 0xF0);
    mem_write(nes, 0x00F0, 0x12);
    mem_write(nes, 0x00F1, 0x90);

    nes->cpu->Y = 0x83;

    uint16_t result = cpu_get_operand(nes, indirect_indexed_with_penalty);
    test_assert_byte_eq(0x9095, result);
    test_assert_int_eq(3, nes->cpu->cycles);
}

TEST(test_get_operand_indirect_indexed_with_penalty_page_crossed) {
    mem_init();

    struct nes* nes = nes_new();
    nes->cpu->PC = 0x020F;
    mem_write(nes, 0x020F, 0xF0);
    mem_write(nes, 0x00F0, 0x12);
    mem_write(nes, 0x00F1, 0x90);

    nes->cpu->Y = 0xF3;

    uint16_t result = cpu_get_operand(nes, indirect_indexed_with_penalty);
    test_assert_byte_eq(0x9105, result);
    test_assert_int_eq(4, nes->cpu->cycles);
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

int main(int argc, char *argv[]) {
  RUN_TEST_SUITE(test_get_operand);
  TEST_REPORT();
  return TEST_EXIT_CODE;
}
