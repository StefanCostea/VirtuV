// src/bindings/virtuv_bindings.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h> // For automatic conversion of STL containers

#include "core/cpu/CPU.hpp"
#include "core/memory/PhysicalMemory.hpp"
#include "core/memory/PageTableEntry.hpp"
#include "core/memory/PageTable.hpp"
#include "core/memory/MMU.hpp"
#include "core/cpu/state/PrivilegeMode.hpp"

namespace py = pybind11;

// Exception translation
void translate_page_fault_exception(const PageFaultException& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
}

void translate_access_violation_exception(const AccessViolationException& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
}

PYBIND11_MODULE(virtuv, m) {
    m.doc() = "VirtuV RISC-V Emulator Python Bindings";

    // Register exception translators
    py::register_exception<PageFaultException>(m, "PageFaultException");
    py::register_exception<AccessViolationException>(m, "AccessViolationException");

    // Bind PrivilegeMode enum
    py::enum_<PrivilegeMode>(m, "PrivilegeMode")
        .value("USER", PrivilegeMode::USER)
        .value("SUPERVISOR", PrivilegeMode::SUPERVISOR)
        .value("MACHINE", PrivilegeMode::MACHINE)
        .export_values();

    // Bind PhysicalMemory class
    py::class_<PhysicalMemory>(m, "PhysicalMemory")
        .def(py::init<size_t>(), py::arg("size"))
        .def("read", &PhysicalMemory::read, "Read a byte from physical memory")
        .def("write", &PhysicalMemory::write, "Write a byte to physical memory");

    // Bind PageTableEntry class
    py::class_<PageTableEntry>(m, "PageTableEntry")
        .def(py::init<uint32_t>(), py::arg("value"))
        .def("is_valid", &PageTableEntry::is_valid)
        .def("is_readable", &PageTableEntry::is_readable, py::arg("mode"))
        .def("is_writable", &PageTableEntry::is_writable, py::arg("mode"))
        .def("is_executable", &PageTableEntry::is_executable, py::arg("mode"))
        .def("get_physical_address", &PageTableEntry::get_physical_address, py::arg("virtual_address"));

    // Bind PageTable class
    py::class_<PageTable>(m, "PageTable")
        .def(py::init<>())
        .def("add_entry", &PageTable::add_entry, py::arg("virtual_address"), py::arg("entry"))
        .def("get_entry", &PageTable::get_entry, py::arg("virtual_address"));

    // Bind MMU class
    py::class_<MMU>(m, "MMU")
        .def(py::init<PhysicalMemory*, PageTable*, PrivilegeMode>(), py::arg("physical_memory"), py::arg("page_table"), py::arg("privilege_mode"))
        .def("read", &MMU::read, "Read a byte from virtual memory", py::arg("virtual_address"))
        .def("write", &MMU::write, "Write a byte to virtual memory", py::arg("virtual_address"), py::arg("value"))
        .def("read_word", &MMU::read_word, "Read a 32-bit word from virtual memory", py::arg("virtual_address"))
        .def("write_word", &MMU::write_word, "Write a 32-bit word to virtual memory", py::arg("virtual_address"), py::arg("value"))
        .def("set_privilege_mode", &MMU::set_privilege_mode, "Set the current privilege mode", py::arg("mode"))
        .def("translate_address", &MMU::translate_address, "Translate a virtual address to a physical address", py::arg("virtual_address"), py::arg("is_write"));
}
