cmake_minimum_required(VERSION 3.16...4.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
project(silph VERSION 1.0
    DESCRIPTION "Pokemon Save Editor"
  LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)
message("Compiler: ${CMAKE_CXX_COMPILER_ID}")


# Build libsilph
add_library(silph)
add_subdirectory(src/libsilph)
target_compile_options(silph PRIVATE
    -Wall
    -Wextra
    -Wno-double-promotion
    -Wno-conversion
    -Wno-sign-conversion
    -Wno-unused-parameter
    -Wno-unused-function
    # -fsanitize=address,undefined
    -g3
)


# Build scope CLI
add_executable(scope)
add_subdirectory(src/scope)
target_compile_options(scope PRIVATE
    -Wall
    -Wextra
    -Wno-double-promotion
    -Wno-conversion
    -Wno-sign-conversion
    -Wno-unused-parameter
    -Wno-unused-function
    # -fsanitize=address,undefined
    -g3
)
# target_link_options(silph PRIVATE
#     -fsanitize=address,undefined
# )
