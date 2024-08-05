cmake_minimum_required(VERSION 3.12)
project(differential_vehicle_model)

set(CMAKE_CXX_STANDARD 14)

find_package(pybind11 REQUIRED)

add_library(differential_vehicle_model MODULE bindings.cpp differentialVehicleModel.cpp)
target_include_directories(differential_vehicle_model PRIVATE ${pybind11_INCLUDE_DIRS})
target_link_libraries(differential_vehicle_model PRIVATE pybind11::module)