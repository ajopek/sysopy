cmake_minimum_required(VERSION 3.9)
project(example C CXX)

set(CMAKE_C_STANDARD 99)

add_executable(mem_err error_memory.c)
add_executable(time_err error_time.c)
