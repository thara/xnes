cmake_minimum_required(VERSION 3.25)

project(xnes LANGUAGES C)

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.c")

add_library(xnes STATIC ${SOURCES})
target_include_directories(xnes PRIVATE ${CMAKE_SOURCE_DIR}/src)
set_target_properties(xnes PROPERTIES C_STANDARD 17)

# code formatter
find_program(CLANG_FORMAT_EXE clang-format)
if(CLANG_FORMAT_EXE)
add_custom_target(
    format
    COMMAND clang-format
    -i
    -style=file
    ${ALL_SOURCES}
    COMMENT "Running clang-format"
    VERBATIM
)
endif()

# linter
find_program(CPPCHECK_EXE cppcheck)
if(CPPCHECK_EXE)
  add_custom_target(
      lint
      COMMAND ${CPPCHECK_EXE}
      -I ${CMAKE_SOURCE_DIR}/src/include
      --enable=warning,performance,portability,style
      --std=c11
      --verbose
      --quiet
      ${SOURCES}
      COMMENT "Running cppcheck"
      VERBATIM
  )
endif()
