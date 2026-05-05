add_executable(gracidea gracidea.h gracidea.cpp
        rs/lib.cpp
        rs/lib.h)
target_sources(gracidea PUBLIC api.h)
target_sources(gracidea PUBLIC cli.cpp cli.h)
add_subdirectory(rs)
target_link_libraries(gracidea PRIVATE junk)
