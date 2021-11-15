cmake_minimum_required(VERSION 3.16)
project(Sokoban_Agent)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

# PyTorch
find_package(Python3 COMPONENTS Interpreter REQUIRED)
set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
file(DOWNLOAD https://raw.githubusercontent.com/python/typing/master/typing_extensions/src_py3/typing_extensions.py ${CMAKE_CURRENT_BINARY_DIR}/python-modules/typing_extensions.py)
include(ExternalProject)
ExternalProject_add(torch
    URL https://github.com/pytorch/pytorch/releases/download/v1.10.0/pytorch-v1.10.0.tar.gz
    CMAKE_ARGS -DBUILD_PYTHON=False -DPYTHON_EXECUTABLE=${Python3_EXECUTABLE} -DCMAKE_PREFIX_PATH=${Python3_SITELIB} -DBUILD_TEST=False -DCMAKE_BUILD_TYPE=Release
)
ExternalProject_Add_Step(torch pip-modules
    COMMAND ${Python3_EXECUTABLE} -m pip install typing_extensions pyyaml
    DEPENDERS configure
)

include_directories(include)

add_subdirectory(lib)
add_subdirectory(exec)

# add_executable(example
#     example.cpp
# )
# target_link_libraries(example
#     ${TORCH_LIBRARIES}
#     mkl_core
# )