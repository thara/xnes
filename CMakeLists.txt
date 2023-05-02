cmake_minimum_required(VERSION 3.25)

project(xnes LANGUAGES C)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.c")

add_executable(xnes ${SOURCES})

target_include_directories(xnes PRIVATE ${CMAKE_SOURCE_DIR}/src/include)

set_target_properties(xnes PROPERTIES C_STANDARD 11)

find_program(CLANG_FORMAT_EXE clang-format)
if(NOT CLANG_FORMAT_EXE)
  message(FATAL_ERROR "clang-format not found. Must be installed.")
endif()
add_custom_target(
    format
    COMMAND clang-format
    -i
    -style=file
    ${SOURCES}
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
