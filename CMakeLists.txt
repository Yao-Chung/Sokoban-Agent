cmake_minimum_required(VERSION 3.16)
project(Sokoban_Agent)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# PyTorch
find_package(Torch QUIET)
if(NOT Torch_FOUND)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
    include(ExternalProject)
    ExternalProject_add(pytorch
        SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/third_party/pytorch
        CMAKE_ARGS -DBUILD_PYTHON=False 
            -DPYTHON_EXECUTABLE=${Python3_EXECUTABLE} 
            -DCMAKE_PREFIX_PATH=${Python3_SITELIB} 
            -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/pytorch 
            -DBUILD_TEST=False 
            -DCMAKE_BUILD_TYPE=Release 
            -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD} -DCMAKE_C_STANDARD=${CMAKE_C_STANDARD}
    )
    ExternalProject_Add_Step(pytorch pip-modules
        COMMAND ${Python3_EXECUTABLE} -m pip install typing_extensions pyyaml
        DEPENDERS configure
    )
    set(Torch_DIR ${CMAKE_BINARY_DIR}/pytorch)
    include_directories(${CMAKE_BINARY_DIR}/pytorch/include)
    include_directories(${CMAKE_BINARY_DIR}/pytorch/include/torch/csrc/api/include)
    link_directories(${Torch_DIR}/lib)
    set(TORCH_LIBRARIES torch torch_cpu c10)
else()
    include_directories(${TORCH_INCLUDE_DIRS})
    set(CMAKE_CXX_FLAGS ${TORCH_CXX_FLAGS} ${CMAKE_CXX_FLAGS})
    add_custom_target(pytorch)
endif()

include_directories(include)

add_subdirectory(lib)
add_subdirectory(exec)

add_executable(example
    example.cpp
)
add_dependencies(example
    pytorch
)
target_link_libraries(example
    ${TORCH_LIBRARIES}
)