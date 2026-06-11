cmake_minimum_required(VERSION 3.16)
project(minilb_python_binding LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(pybind11 REQUIRED)

pybind11_add_module(minilb_py minilb_py.cpp)
