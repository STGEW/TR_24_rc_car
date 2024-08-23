#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "differentialVehicleModel.h"


namespace py = pybind11;


PYBIND11_MODULE(differential_vehicle_model, m) {
    py::class_<Vehicle2DPosition>(m, "Vehicle2DPosition")
        .def(py::init<>())
        .def_readwrite("x", &Vehicle2DPosition::x)
        .def_readwrite("y", &Vehicle2DPosition::y)
        .def_readwrite("phi", &Vehicle2DPosition::phi);

    py::class_<PathChunk>(m, "PathChunk")
        .def(py::init<>())
        .def_readwrite("x1", &PathChunk::x1)
        .def_readwrite("y1", &PathChunk::y1)
        .def_readwrite("phi1", &PathChunk::phi1)
        .def_readwrite("x2", &PathChunk::x2)
        .def_readwrite("y2", &PathChunk::y2);

    py::class_<OdometerValues>(m, "OdometerValues")
        .def(py::init<>())
        .def_readwrite("phi2", &OdometerValues::phi2)
        .def_readwrite("angle_n_l", &OdometerValues::angle_n_l)
        .def_readwrite("angle_n_r", &OdometerValues::angle_n_r)
        .def_readwrite("dist_n", &OdometerValues::dist_n);

    py::class_<DifferentialVehicleModel>(m, "DifferentialVehicleModel")
        .def(py::init<float, float, int>())
        .def("odometer_to_position_diff", &DifferentialVehicleModel::odometer_to_position_diff)
        .def("path_to_odometer_values", &DifferentialVehicleModel::path_to_odometer_values)
        .def("reset_internal_state", &DifferentialVehicleModel::reset_internal_state)
        .def("dist_diff_to_odo", &DifferentialVehicleModel::dist_diff_to_odo)
        .def("yaw_diff_to_odo", &DifferentialVehicleModel::yaw_diff_to_odo)
        .def("odo_to_pos_rotate", &DifferentialVehicleModel::odo_to_pos_rotate)
        .def("odo_to_pos_linear", &DifferentialVehicleModel::odo_to_pos_linear);
}

