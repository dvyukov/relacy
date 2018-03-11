cmake_minimum_required(VERSION 2.8.12)

enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra")

add_subdirectory(g++)
add_subdirectory(jtest)
add_subdirectory(ntest)

add_executable(main main.cpp)
add_executable(iriw iriw.cpp)

add_test(main main)
add_test(iriw iriw)
