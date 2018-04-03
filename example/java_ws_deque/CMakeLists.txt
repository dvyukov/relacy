cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra")

add_executable(java_ws_deque java_ws_deque.cpp stdafx.cpp)
