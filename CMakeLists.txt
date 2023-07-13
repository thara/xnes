cmake_minimum_required(VERSION 3.25)

project(xnes LANGUAGES C)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.c")

add_library(xnes STATIC ${SOURCES})
target_include_directories(xnes PRIVATE ${CMAKE_SOURCE_DIR}/src)
set_target_properties(xnes PROPERTIES C_STANDARD 11)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

enable_testing()
set(CMAKE_CTEST_ARGUMENTS "--verbose")

add_subdirectory(tests)

file(GLOB_RECURSE TEST_SOURCES "${CMAKE_SOURCE_DIR}/tests/*.c")

file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/src/include/*.h")

file(GLOB_RECURSE ALL_SOURCES
  ${SOURCES}
  ${TEST_SOURCES}
  ${HEADERS}
)

# SDL2
find_package(SDL2 REQUIRED)
if(SDL2_FOUND)
  include_directories(${SDL2_INCLUDE_DIRS})
  add_executable(xnes_sdl "${SOURCES}" src/sdl/main.c)
  set_target_properties(xnes_sdl PROPERTIES C_STANDARD 11)

  target_include_directories(xnes_sdl PRIVATE ${CMAKE_SOURCE_DIR}/src/sdl)
  target_link_libraries(xnes_sdl ${SDL2_LIBRARIES})
endif()

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
