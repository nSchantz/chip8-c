cmake_minimum_required(VERSION 3.10)
project(Chip8_EMU)

add_executable(
  Chip8_EMU
  emu.c
  internals.c
  peripheral.c
)

target_include_directories(
  Chip8_EMU PUBLIC
  "./ref/"
)
