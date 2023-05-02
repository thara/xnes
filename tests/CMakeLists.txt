macro(ADD_TEST_MACRO NAME SOURCE)
  add_executable("${NAME}" "${SOURCE}")
  target_link_libraries("${NAME}" xnes)
  target_include_directories("${NAME}" PRIVATE ${CMAKE_SOURCE_DIR}/src/include)
  set_target_properties("${NAME}" PROPERTIES C_STANDARD 11)
  add_test("${NAME}" "${NAME}")
endmacro()

ADD_TEST_MACRO(cpu_test test_cpu.c)
