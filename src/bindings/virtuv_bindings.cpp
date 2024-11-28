#include <pybind11/pybind11.h>
#include "CPU.hpp"

namespace py = pybind11;

PYBIND11_MODULE(virtuv, m) {
    m.doc() = "VirtuV RISC-V Emulator Python Bindings";

    py::class_<Cpu>(m, "CPU")
        .def(py::init<size_t>(), py::arg("memory_size"))
        .def("load_program", &Cpu::loadProgram, "Load a RISC-V binary program")
        .def("run", &CPU::run, "Run the CPU");

    py::class_<RegisterBank>(m, "RegisterBank")
        .def("read", &RegisterBank::read, "Read a value from a register")
        .def("write", &RegisterBank::write, "Write a value to a register");
}
