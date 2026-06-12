add_executable(silph silph.h silph.cpp
        rs/lib.cpp
        rs/lib.h)
target_sources(silph PUBLIC api.h)
target_sources(silph PUBLIC cli.cpp cli.h)
add_subdirectory(rs)
target_link_libraries(silph PRIVATE junk)
