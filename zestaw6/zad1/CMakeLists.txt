cmake_minimum_required(VERSION 3.10)
project(zestaw6 C)

set(CMAKE_C_STANDARD 99)

add_executable(server main.c communication_protocol.h)
add_executable(client client.c communication_protocol.h)