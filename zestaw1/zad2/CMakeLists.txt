cmake_minimum_required(VERSION 3.5.1)
project(zestaw1 C CXX)

set(CMAKE_C_STANDARD 11)

add_executable(zad2
        main.c
        ../zad1/string_array_lib.c
        ../zad1/string_array_lib.h)
