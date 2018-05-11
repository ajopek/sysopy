cmake_minimum_required(VERSION 3.10)
project(zestaw7 C)

set(CMAKE_C_STANDARD 99)

add_executable(barber main.c barber_problem.h)
add_executable(client client.c barber_problem.h)