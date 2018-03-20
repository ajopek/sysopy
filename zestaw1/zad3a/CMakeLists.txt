cmake_minimum_required(VERSION 3.9)

SET(GCC_COVERAGE_COMPILE_FLAGS "-Os")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

# Declare project
project(zestaw1 C CXX)

# Set vars
set(CMAKE_C_STANDARD 99)

# Set sources
set(ZAD1_SOURCE_FILES ../zad1/string_array_lib.c ../zad1/string_array_lib.h)
set(ZAD2_SOURCE_FILES ../zad2/main.c)

# Declare variants of the library
add_library(zad1_static STATIC ${ZAD1_SOURCE_FILES})
add_library(zad1_shared SHARED ${ZAD1_SOURCE_FILES})

# Declare executables
add_executable(zad2_with_static ${ZAD2_SOURCE_FILES})
add_executable(zad2_with_shared ${ZAD2_SOURCE_FILES})
add_executable(zad2_with_dynamic ${ZAD2_SOURCE_FILES})


set_target_properties(zad2_with_dynamic
        PROPERTIES COMPILE_FLAGS "-D DYNAMIC_LOADING -fPIC  -ldl")
target_link_libraries(zad2_with_dynamic ${CMAKE_DL_LIBS})
target_link_libraries(zad2_with_static zad1_static)
target_link_libraries(zad2_with_shared zad1_shared)
