cmake_minimum_required(VERSION 3.16...4.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(gracidea VERSION 1.0
    DESCRIPTION "Pokemon Save Editor"
  LANGUAGES C)

message("Compiler: ${CMAKE_CXX_COMPILER_ID}")

add_subdirectory(src)
