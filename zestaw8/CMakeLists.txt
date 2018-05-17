cmake_minimum_required(VERSION 3.10)
project(zestaw8 C)

set(CMAKE_C_STANDARD 99)

add_executable(main main.c)

target_link_libraries(main m)
target_link_libraries(main pthread)