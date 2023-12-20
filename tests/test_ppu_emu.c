#include "test_common.h"

#define UNIT_TEST

#include "ppu_emu.h"

#include "nes.h"
#include "nes_emu.h"
#include "mapper.h"

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

TEST_SUITE(test_ppu_emu) {
  RUN_TEST(test_ppu_coarse_x);
  RUN_TEST(test_ppu_coarse_y);
  RUN_TEST(test_ppu_fine_y);
  RUN_TEST(test_ppu_tile_addr);
  RUN_TEST(test_ppu_attr_addr);
}

Mapper *mock_mapper_new(uint8_t mapper_no, MirroringMode mirroring);

TEST(test_PPUCTRL) {
  NES *nes = nes_new();
  Mapper *mapper = mock_mapper_new(0, MIRRORING_HORIZONTAL);
  nes_insert_cartridge(nes, mapper);

  uint16_t addr = 0x2000;

  ppu_write_register(nes, addr, 0b01010000);
  test_assert(ppu_ctrl_enabled(&nes->ppu, PPUCTRL_SLAVE), "slave should be on");
  test_assert(ppu_ctrl_enabled(&nes->ppu, PPUCTRL_BG_TABLE),
              "bg table should be on");

  ppu_write_register(nes, addr, 0b00000111);
  test_assert(ppu_ctrl_enabled(&nes->ppu, PPUCTRL_VRAM_INCR),
              "VRAM increments should be on");
  test_assert_bit_eq(0b11, nes->ppu.ctrl & PPUCTRL_NT);

  nes_release(nes);
}

TEST(test_PPUMASK) {
  NES *nes = nes_new();
  Mapper *mapper = mock_mapper_new(0, MIRRORING_HORIZONTAL);
  nes_insert_cartridge(nes, mapper);

  uint16_t addr = 0x2001;

  ppu_write_register(nes, addr, 0b01010000);
  test_assert(ppu_mask_enabled(&nes->ppu, PPUMASK_GREEN), "green should be on");
  test_assert(ppu_mask_enabled(&nes->ppu, PPUMASK_SPR), "spr should be on");

  ppu_write_register(nes, addr, 0b00000111);
  test_assert(ppu_mask_enabled(&nes->ppu, PPUMASK_SPR_LEFT),
              "spr left should be on");
  test_assert(ppu_mask_enabled(&nes->ppu, PPUMASK_BG_LEFT),
              "bg left should be on");
  test_assert(ppu_mask_enabled(&nes->ppu, PPUMASK_GRAYSCALE),
              "grayscale should be on");

  nes_release(nes);
}

TEST(test_PPUSTATUS) {
  NES *nes = nes_new();
  Mapper *mapper = mock_mapper_new(0, MIRRORING_HORIZONTAL);
  nes_insert_cartridge(nes, mapper);

  uint16_t addr = 0x2002;

  nes->ppu.status = 0b11000000;
  nes->ppu.w = true;

  test_assert_bit_eq(0b11000000, ppu_read_register(nes, addr));
  test_assert(nes->ppu.w == false, "w should be false");
  test_assert_bit_eq(0b01000000, ppu_read_register(nes, addr));

  nes_release(nes);
}

TEST(test_OAMDATA) {
  NES *nes = nes_new();
  Mapper *mapper = mock_mapper_new(0, MIRRORING_HORIZONTAL);
  nes_insert_cartridge(nes, mapper);

  uint16_t addr = 0x2003;

  ppu_write_register(nes, addr, 255);
  test_assert_byte_eq(255, nes->ppu.oam_addr);

  nes_release(nes);
}

TEST(test_OAMDATA_read) {
  NES *nes = nes_new();
  Mapper *mapper = mock_mapper_new(0, MIRRORING_HORIZONTAL);
  nes_insert_cartridge(nes, mapper);

  uint16_t addr = 0x2004;

  nes->ppu.spr.oam[0x09] = 0xA3;
  ppu_write_register(nes, 0x2003, 0x09);

  test_assert_byte_eq(0xA3, ppu_read_register(nes, addr));

  nes_release(nes);
}

TEST(test_OAMDATA_write) {
  NES *nes = nes_new();
  Mapper *mapper = mock_mapper_new(0, MIRRORING_HORIZONTAL);
  nes_insert_cartridge(nes, mapper);

  uint16_t addr = 0x2004;

  ppu_write_register(nes, 0x2003, 0xAB);
  ppu_write_register(nes, addr, 0x32);

  test_assert_byte_eq(0x32, nes->ppu.spr.oam[0xAB]);

  nes_release(nes);
}

TEST(test_PPUSCROLL) {
  NES *nes = nes_new();
  Mapper *mapper = mock_mapper_new(0, MIRRORING_HORIZONTAL);
  nes_insert_cartridge(nes, mapper);

  uint16_t addr = 0x2005;

  ppu_write_register(nes, addr, 0x1F);

  test_assert(nes->ppu.w, "w should be true");
  test_assert_byte_eq(3, ppu_coarse_x(nes->ppu.t));
  test_assert_bit_eq(0b111, nes->ppu.x);

  ppu_write_register(nes, addr, 0x0E);
  test_assert(nes->ppu.w == false, "w should be false");
  test_assert_byte_eq(1, ppu_coarse_y(nes->ppu.t));

  nes_release(nes);
}

TEST(test_PPUADDR) {
  NES *nes = nes_new();
  Mapper *mapper = mock_mapper_new(0, MIRRORING_HORIZONTAL);
  nes_insert_cartridge(nes, mapper);

  uint16_t addr = 0x2006;

  nes->ppu.v = 0;
  nes->ppu.t = 0;

  ppu_write_register(nes, addr, 0x3F);
  test_assert(nes->ppu.w, "w should be true");

  ppu_write_register(nes, addr, 0x91);
  test_assert(nes->ppu.w == false, "w should be false");

  test_assert_byte_eq(0x3F91, nes->ppu.v);
  test_assert_byte_eq(0x3F91, nes->ppu.t);

  nes_release(nes);
}

TEST(test_PPUDATA) {
  NES *nes = nes_new();
  Mapper *mapper = mock_mapper_new(0, MIRRORING_HORIZONTAL);
  nes_insert_cartridge(nes, mapper);

  uint16_t addr = 0x2007;

  ppu_write_register(nes, 0x2006, 0x2F);
  ppu_write_register(nes, 0x2006, 0x11);

  ppu_write_register(nes, addr, 0x83);

  test_assert_byte_eq(0x83, ppu_read(nes, 0x2F11));

  nes_release(nes);
}

TEST_SUITE(test_ppu_io) {
  RUN_TEST(test_PPUCTRL);
  RUN_TEST(test_PPUMASK);
  RUN_TEST(test_PPUSTATUS);
  RUN_TEST(test_OAMDATA);
  RUN_TEST(test_OAMDATA_read);
  RUN_TEST(test_OAMDATA_write);
  RUN_TEST(test_PPUSCROLL);
  RUN_TEST(test_PPUADDR);
  RUN_TEST(test_PPUDATA);
}

int main(int argc, char *argv[]) {
  RUN_TEST_SUITE(test_ppu_emu);
  RUN_TEST_SUITE(test_ppu_io);
  TEST_REPORT();
  return TEST_EXIT_CODE;
}
