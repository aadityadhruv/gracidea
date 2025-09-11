add_executable(gracidea gracidea.h gracidea.c)
add_subdirectory(rs)
target_link_libraries(gracidea PRIVATE junk)
