cmake_minimum_required(VERSION 3.16)
project(Sokoban_Agent)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_subdirectory(lib)
add_subdirectory(exec)