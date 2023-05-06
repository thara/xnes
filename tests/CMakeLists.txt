file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.c")

list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/src/mem.c")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/tests/mock_mem.c")

add_library(mocked_xnes STATIC ${SOURCES})
target_include_directories(mocked_xnes PRIVATE ${CMAKE_SOURCE_DIR}/src/include)
target_include_directories(mocked_xnes PRIVATE ${CMAKE_SOURCE_DIR}/tests)
set_target_properties(mocked_xnes PROPERTIES C_STANDARD 11)
target_compile_definitions(mocked_xnes PRIVATE UNIT_TEST)

set(TEST_DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/testdata")

macro(XNES_ADD_TEST NAME SOURCE)
  add_executable("${NAME}" "${SOURCE}" "mock_mem.c")
  target_link_libraries("${NAME}" mocked_xnes)
  target_include_directories("${NAME}" PRIVATE ${CMAKE_SOURCE_DIR}/src/include)
  target_include_directories("${NAME}" PRIVATE ${CMAKE_SOURCE_DIR}/tests)
  target_compile_definitions("${NAME}" PRIVATE TEST_DATA_PATH="${TEST_DATA_PATH}")
  set_target_properties("${NAME}" PROPERTIES C_STANDARD 11)
  add_test("${NAME}" "${NAME}")
endmacro()

XNES_ADD_TEST(cpu_test test_cpu.c)
XNES_ADD_TEST(cpu_emulation_test test_cpu_emulation.c)
XNES_ADD_TEST(rom_test test_rom.c)
