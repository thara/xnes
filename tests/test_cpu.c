#include "test_common.h"

#include "cpu.h"
#include "mem.h"

TEST(test_cpu_status_set) {
    struct cpu cpu;
    cpu.P = 0b11001100;
    cpu_status_set(&cpu, V, true);

    test_assert_int_eq(0b11011100, cpu.P);
}

TEST(test_cpu_status_enabled) {
    struct cpu cpu;
    cpu.P = 0b11001100;

    test_assert(cpu_status_enabled(&cpu, D), "decimal flag should be true");
    test_assert(!cpu_status_enabled(&cpu, C), "carry flag should be false");
}

TEST(test_cpu_status_set_zn) {
    struct cpu cpu;
    cpu.P = 0b11001100;
    cpu_status_set_zn(&cpu, 0);

    test_assert(cpu_status_enabled(&cpu, Z), "zero flag should be on");
    test_assert(!cpu_status_enabled(&cpu, N), "negative flag should be off");
}

TEST_SUITE(test_cpu_status) {
    RUN_TEST(test_cpu_status_set);
    RUN_TEST(test_cpu_status_enabled);
    RUN_TEST(test_cpu_status_set_zn);
}

int main(int argc, char *argv[]) {
  RUN_TEST_SUITE(test_cpu_status);
  TEST_REPORT();
  return TEST_EXIT_CODE;
}
