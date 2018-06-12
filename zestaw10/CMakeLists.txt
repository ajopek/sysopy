cmake_minimum_required(VERSION 2.6)
project(lab10 C)

set(CMAKE_C_STANDARD 99)

add_executable(server1 ./zad1/server.c ./zad1/common.h)
add_executable(client1 ./zad1/client.c ./zad1/common.h)

target_link_libraries(server1 pthread rt m)
target_link_libraries(client1 pthread rt m)

add_executable(server2 zad2/server.c zad2/common.h)
add_executable(client2 zad2/client.c zad2/common.h)

target_link_libraries(server2 pthread rt m)
target_link_libraries(client2 pthread rt m)