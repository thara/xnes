cmake_minimum_required(VERSION 3.25)

project(xnes LANGUAGES C)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.c")

add_library(xnes STATIC ${SOURCES})
target_include_directories(xnes PRIVATE ${CMAKE_SOURCE_DIR}/src/include)
set_target_properties(xnes PROPERTIES C_STANDARD 11)
# add_executable(xnes ${SOURCES})

enable_testing()
set(CMAKE_CTEST_ARGUMENTS "--verbose")

add_subdirectory(tests)

file(GLOB_RECURSE TEST_SOURCES "${CMAKE_SOURCE_DIR}/tests/*.c")

# add_executable(xnes_test ${SOURCES} ${TEST_SOURCES})
# target_include_directories(xnes_test PRIVATE ${CMAKE_SOURCE_DIR}/src/include)
# target_include_directories(xnes_test PRIVATE ${CMAKE_SOURCE_DIR}/src)
# target_include_directories(xnes_test PRIVATE ${CMAKE_SOURCE_DIR}/tests)
# set_target_properties(xnes_test PROPERTIES C_STANDARD 11)

# add_test(NAME run_test COMMAND xnes_test)

file(GLOB_RECURSE ALL_SOURCES
  ${SOURCES}
  ${TEST_SOURCES}
)

find_program(CLANG_FORMAT_EXE clang-format)
if(NOT CLANG_FORMAT_EXE)
  message(FATAL_ERROR "clang-format not found. Must be installed.")
endif()
add_custom_target(
    format
    COMMAND clang-format
    -i
    -style=file
    ${ALL_SOURCES}
    COMMENT "Running clang-format"
    VERBATIM
)

find_program(CPPCHECK_EXE cppcheck)
if(NOT CPPCHECK_EXE)
  message(FATAL_ERROR "cppcheck not found. Must be installed.")
endif()

add_custom_target(
    lint
    COMMAND ${CPPCHECK_EXE}
    --enable=warning,performance,portability,style
    --std=c11
    --verbose
    --quiet
    ${SOURCES}
    COMMENT "Running cppcheck"
    VERBATIM
)
