#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include "commandsParser.h"

namespace py = pybind11;


// Wrapper functions to convert std::function to raw function pointers
void new_path_cb_wrapper(Point2D* point_2D, std::function<void(Point2D*)>* cb) {
    (*cb)(point_2D);
}

void stop_cb_wrapper(std::function<void(void)>* cb) {
    (*cb)();
}

PYBIND11_MODULE(commands_parser, m) {
    py::class_<Point2D>(m, "Point2D")
        .def(py::init<>())
        .def_readwrite("x", &Point2D::x)
        .def_readwrite("y", &Point2D::y);

    py::enum_<ParserState>(m, "ParserState")
        .value("HEADER", HEADER)
        .value("SIZE", SIZE)
        .value("BODY", BODY)
        .export_values();

    py::class_<CommandsParser>(m, "CommandsParser")
        .def(py::init([](std::function<void(Point2D*)> new_path_cb, std::function<void(void)> stop_cb) {
            // Create dynamic objects to hold the std::function callbacks
            auto new_path_cb_ptr = new std::function<void(Point2D*)>(new_path_cb);
            auto stop_cb_ptr = new std::function<void(void)>(stop_cb);

            // Return the CommandsParser instance using the wrapper functions
            return new CommandsParser(
                reinterpret_cast<void (*)(Point2D*)>(new_path_cb_wrapper),
                reinterpret_cast<void (*)(void)>(stop_cb_wrapper)
            );
        }))
        .def("parse", &CommandsParser::parse)
        .def("reset", &CommandsParser::reset);
}