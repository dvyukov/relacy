cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_FALGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra")

add_executable(ntest ntest.cpp)

add_test(ntest ntest)
