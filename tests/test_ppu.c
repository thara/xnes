#include "test_common.h"

#include "ppu.h"

TEST(test_ppu_coarse_x) {
  test_assert_bit_eq(0b11101, ppu_coarse_x(0b1101101100111101));
}

TEST(test_ppu_coarse_y) {
  test_assert_bit_eq(0b11001, ppu_coarse_y(0b1101101100111101));
}

TEST(test_ppu_fine_y) {
  test_assert_bit_eq(0b101, ppu_fine_y(0b1101101100111101));
}

TEST(test_ppu_tile_addr) {
  test_assert_bit_eq(0b10101100111101, ppu_tile_addr(0b1101101100111101));
}

TEST(test_ppu_attr_addr) {
  test_assert_bit_eq(0b10101111110111, ppu_attr_addr(0b1101101100111101));
}

TEST_SUITE(test_ppu) {
  RUN_TEST(test_ppu_coarse_x);
  RUN_TEST(test_ppu_coarse_y);
  RUN_TEST(test_ppu_fine_y);
  RUN_TEST(test_ppu_tile_addr);
  RUN_TEST(test_ppu_attr_addr);
}

int main(int argc, char *argv[]) {
  RUN_TEST_SUITE(test_ppu);
  TEST_REPORT();
  return TEST_EXIT_CODE;
}
