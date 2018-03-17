cmake_minimum_required(VERSION 3.5.1)
project(zad1 C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Release)

file(GLOB SOURCES "*.c")

add_library(string_array SHARED ${SOURCES})
add_library(string_array_static STATIC ${SOURCES})
