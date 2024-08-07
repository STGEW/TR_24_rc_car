cmake_minimum_required(VERSION 3.12)
project(commands_parser)

set(CMAKE_CXX_STANDARD 14)

find_package(pybind11 REQUIRED)

add_library(commands_parser MODULE bindings.cpp commandsParser.cpp)
target_include_directories(commands_parser PRIVATE ${pybind11_INCLUDE_DIRS})
target_link_libraries(commands_parser PRIVATE pybind11::module)