cmake_minimum_required(VERSION 3.10)
project(Chip8_EMU)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

add_executable(
  Chip8_EMU
  emu.c
  internals.c
  peripheral.c
)

if (TARGET SDL2::SDL2main)
  target_link_libraries(Chip8_EMU PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(Chip8_EMU PRIVATE SDL2::SDL2)

target_include_directories(
  Chip8_EMU PUBLIC
  "./ref/"
)
