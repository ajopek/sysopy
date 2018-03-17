cmake_minimum_required(VERSION 3.5.1)
project(zad2 C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Release)
set(LIB_SOURCES string_array_lib.c)
set(EXEC_SOURCES main.c)

add_executable(zad2 ${EXEC_SOURCES})
target_link_libraries(zad2 ${CMAKE_CURRENT_SOURCE_DIR}/libstring_array_static.a)
