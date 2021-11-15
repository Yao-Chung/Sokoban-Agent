cmake_minimum_required(VERSION 3.16)
project(Sokoban_Agent)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# PyTorch
find_package(Torch REQUIRED)
include_directories(${TORCH_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS ${TORCH_CXX_FLAGS} ${CMAKE_CXX_FLAGS})

include_directories(include)

add_subdirectory(lib)
add_subdirectory(exec)

add_executable(example
    example.cpp
)
target_link_libraries(example
    ${TORCH_LIBRARIES}
)