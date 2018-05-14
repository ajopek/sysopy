cmake_minimum_required(VERSION 3.10)
project(zestaw7 C)

set(CMAKE_C_STANDARD 99)

add_executable(barber barber2.c common.h)
add_executable(client client2.c common.h)