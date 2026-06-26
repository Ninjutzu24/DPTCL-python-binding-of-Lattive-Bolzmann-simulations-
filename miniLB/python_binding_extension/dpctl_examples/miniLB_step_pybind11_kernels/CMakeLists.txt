cmake_minimum_required(VERSION 3.18)
project(minilb_step_pybind11 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 REQUIRED)

execute_process(
    COMMAND ${Python3_EXECUTABLE} -c "import dpctl, sys; sys.stdout.write(dpctl.get_include())"
    OUTPUT_VARIABLE DPCTL_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "dpctl include dir: ${DPCTL_INCLUDE_DIR}")

pybind11_add_module(_minilb_step _minilb_step.cpp)

target_include_directories(_minilb_step PRIVATE ${DPCTL_INCLUDE_DIR})

target_compile_options(_minilb_step PRIVATE -fsycl)
target_link_options(_minilb_step PRIVATE -fsycl)
