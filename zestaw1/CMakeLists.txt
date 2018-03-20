cmake_minimum_required(VERSION 3.9)
project(sysopy)

set(CMAKE_CXX_STANDARD 11)

add_executable(sysopy
        main.c
        string_array_lib.c
        string_array_lib.h)
