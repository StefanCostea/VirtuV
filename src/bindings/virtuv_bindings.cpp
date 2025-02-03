#include <pybind11/pybind11.h>
#include <pybind11/stl.h> 


#include "core/cpu/CPU.hpp"
#include "core/cpu/register_bank/RegisterBank.hpp"
#include "core/cpu/isa/Instruction.hpp" 
#include "core/cpu/pipeline/Pipeline.hpp"
#include "core/cpu/pipeline/fetch/FetchStage.hpp"
#include "core/cpu/pipeline/decode/DecodeStage.hpp"
#include "core/cpu/pipeline/execute/ExecuteStage.hpp"
#include "core/cpu/pipeline/memory_access/MemoryAccessStage.hpp"
#include "core/cpu/pipeline/write_back/WriteBackStage.hpp"
#include "core/memory/PhysicalMemory.hpp"
#include "core/memory/PageTable.hpp"
#include "core/memory/PageTableEntry.hpp"
#include "core/memory/MMU.hpp"
#include "core/cpu/state/PrivilegeMode.hpp"

using DecodedInstructionInvalid   = DecodedInstruction<InstructionFormat::INIVALID_TYPE>;
using DecodedInstructionRType     = DecodedInstruction<InstructionFormat::R_TYPE>;
using DecodedInstructionIType     = DecodedInstruction<InstructionFormat::I_TYPE>;
using DecodedInstructionSType     = DecodedInstruction<InstructionFormat::S_TYPE>;
using DecodedInstructionBType     = DecodedInstruction<InstructionFormat::B_TYPE>;
using DecodedInstructionUType     = DecodedInstruction<InstructionFormat::U_TYPE>;
using DecodedInstructionJType     = DecodedInstruction<InstructionFormat::J_TYPE>;

namespace py = pybind11;


namespace detail {

  template<typename T>
  std::optional<std::variant<T>> try_cast_variant_impl(pybind11::object obj) {
      if (pybind11::isinstance<T>(obj)) {
          return std::variant<T>(std::in_place_type<T>, obj.cast<T>());
      }
      return std::nullopt;
  }

  template<typename T, typename... Ts> requires (sizeof...(Ts) > 0)
  std::optional<std::variant<T, Ts...>> try_cast_variant_impl(pybind11::object obj) {
      if (pybind11::isinstance<T>(obj)) {
          return std::variant<T, Ts...>(std::in_place_type<T>, obj.cast<T>());
      }
      auto opt = try_cast_variant_impl<Ts...>(obj);
      if (opt) {
          // Convert std::variant<Ts...> to std::variant<T, Ts...>
          return std::visit([](auto&& val) -> std::variant<T, Ts...> {
              using U = std::decay_t<decltype(val)>;
              return std::variant<T, Ts...>(std::in_place_type<U>, std::forward<decltype(val)>(val));
          }, *opt);
      }
      return std::nullopt;
  }
}

template<typename... Ts>
std::variant<Ts...> try_cast_variant(pybind11::object obj) {
    auto opt = detail::try_cast_variant_impl<Ts...>(obj);
    if (opt)
        return *opt;
    throw std::runtime_error("Unkwon cast type");
}

// Exception translation
void translate_page_fault_exception(const PageFaultException& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
}

void translate_access_violation_exception(const AccessViolationException& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
}

PYBIND11_MODULE(virtuv_bindings, m) {
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

    
    // Bind RegisterBank
    py::class_<RegisterBank>(m, "RegisterBank")
        .def(py::init<>())
        .def("read", &RegisterBank::read, "Read a register", py::arg("reg"))
        .def("write", &RegisterBank::write, "Write a register", py::arg("reg"), py::arg("value"))
        .def("get_pc", &RegisterBank::get_pc, "Get the current program counter")
        .def("set_pc", &RegisterBank::set_pc, "Set the program counter", py::arg("value"));

    // Bind CPU
    py::class_<CPU>(m, "CPU")
        .def(py::init<size_t>(), py::arg("memory_size"))
        .def("load_program", &CPU::load_program, "Load a binary program into memory", py::arg("filepath"))
        .def("run", &CPU::run, "Run the CPU")
        .def("get_register", &CPU::get_register, "Read a given general purpose value");

    // Bind pipeline
    py::class_<Pipeline>(m, "Pipeline")
        .def(py::init<RegisterBank&, MMU&>(), py::arg("register_bank"), py::arg("mmu"))
        .def("run_cycle", &Pipeline::run_cycle, "Run one cycle of the pipeline");

    // Bind FetchStage
    py::class_<FetchStage>(m, "FetchStage")
        .def(py::init<MMU&, RegisterBank&>(), py::arg("mmu"), py::arg("register_bank"))
        .def("process", &FetchStage::process, "Process the fetch stage")
        .def("get_fetched_instruction", &FetchStage::get_fetched_instruction, "Return the fetched instruction");

    // Bind DecodeStage
    py::class_<DecodeStage>(m, "DecodeStage")
        .def(py::init<RegisterBank&>(), py::arg("register_bank"))
        .def("set_fetched_instruction", &DecodeStage::set_fetched_instruction, "Set the fetched instruction", py::arg("instruction"))
        .def("process", &DecodeStage::process, "Process the decode stage")
        .def("get_decoded_instruction", &DecodeStage::get_decoded_instruction, "Return the decoded instruction variant");

    // Bind ExecuteStage
    py::class_<ExecuteStage>(m, "ExecuteStage")
        .def(py::init<RegisterBank&>(), py::arg("register_bank"))
        .def("set_decoded_instruction",
         [](ExecuteStage &self, pybind11::object decoded_obj) {
             auto var = try_cast_variant<DecodedInstructionInvalid,
                                         DecodedInstructionRType,
                                         DecodedInstructionIType,
                                         DecodedInstructionSType,
                                         DecodedInstructionBType,
                                         DecodedInstructionUType,
                                         DecodedInstructionJType>(decoded_obj);
             self.set_decoded_instruction(std::move(var));
         },
         py::arg("decoded_instruction"), "Set the decoded instruction")
        .def("process", &ExecuteStage::process, "Process the execute stage")
        .def("get_result", &ExecuteStage::get_result, "Get the execution result");

    // Bind MemoryAccessStage
    py::class_<MemoryAccessStage>(m, "MemoryAccessStage")
        .def(py::init<MMU&, RegisterBank&>(), py::arg("mmu"), py::arg("register_bank"))
        .def("set_execution_result", &MemoryAccessStage::set_execution_result, "Set the execution result", py::arg("exec_result"))
        .def("set_decoded_instruction",
         [](ExecuteStage &self, pybind11::object decoded_obj) {
             auto var = try_cast_variant<DecodedInstructionInvalid,
                                         DecodedInstructionRType,
                                         DecodedInstructionIType,
                                         DecodedInstructionSType,
                                         DecodedInstructionBType,
                                         DecodedInstructionUType,
                                         DecodedInstructionJType>(decoded_obj);
             self.set_decoded_instruction(std::move(var));
         },
         py::arg("decoded_instruction"), "Set the decoded instruction")
        .def("process", &MemoryAccessStage::process, "Process the memory access stage")
        .def("get_result", &MemoryAccessStage::get_result, "Get the memory access result");

    // Bind WriteBackStage
    py::class_<WriteBackStage>(m, "WriteBackStage")
        .def(py::init<RegisterBank&>(), py::arg("register_bank"))
        .def("set_execution_result", &WriteBackStage::set_execution_result, "Set the execution result", py::arg("exec_result"))
        .def("set_memory_access_result", &WriteBackStage::set_memory_access_result, "Set the memory access result", py::arg("mem_result"))
        .def("set_decoded_instruction",
         [](ExecuteStage &self, pybind11::object decoded_obj) {
             auto var = try_cast_variant<DecodedInstructionInvalid,
                                         DecodedInstructionRType,
                                         DecodedInstructionIType,
                                         DecodedInstructionSType,
                                         DecodedInstructionBType,
                                         DecodedInstructionUType,
                                         DecodedInstructionJType>(decoded_obj);
             self.set_decoded_instruction(std::move(var));
         },
         py::arg("decoded_instruction"), "Set the decoded instruction")
         .def("process", &WriteBackStage::process, "Process the write-back stage");

    // Bind interstage communication objects
    py::class_<ExecutionResult>(m, "ExecutionResult")
        .def(py::init<>())
        .def_readwrite("alu_result", &ExecutionResult::alu_result, "ALU result computed during execution");
    
    py::class_<MemoryAccessResult>(m, "MemoryAccessResult")
        .def(py::init<>())
        .def_readwrite("load_data", &MemoryAccessResult::load_data, "Loaded data from memory (if any)")
        .def_readwrite("store_success", &MemoryAccessResult::store_success, "Indicates if a store was successful");

    // Bind the InstructionFormat enum.
    py::enum_<InstructionFormat>(m, "InstructionFormat")
    .value("INIVALID_TYPE", InstructionFormat::INIVALID_TYPE)
    .value("R_TYPE", InstructionFormat::R_TYPE)
    .value("I_TYPE", InstructionFormat::I_TYPE)
    .value("S_TYPE", InstructionFormat::S_TYPE)
    .value("B_TYPE", InstructionFormat::B_TYPE)
    .value("U_TYPE", InstructionFormat::U_TYPE)
    .value("J_TYPE", InstructionFormat::J_TYPE)
    .export_values();

    // Bind each specialization of DecodedInstruction.  (bit fields are not addressabl because of memory alignment, lambdas are needed to modify individually)

    // Invalid type (for initialization or error)
    py::class_<DecodedInstruction<InstructionFormat::INIVALID_TYPE>>(m, "DecodedInstructionInvalid")
        .def(py::init<uint32_t>())
        .def("get_opcode", &DecodedInstruction<InstructionFormat::INIVALID_TYPE>::get_opcode);

    // R-type instructions
    py::class_<DecodedInstruction<InstructionFormat::R_TYPE>>(m, "DecodedInstructionRType")
        .def(py::init<uint32_t>())
        .def_property("opcode",
         [](const DecodedInstruction<InstructionFormat::R_TYPE>& inst) {
             return inst.opcode;
         },
         [](DecodedInstruction<InstructionFormat::R_TYPE>& inst, uint32_t value) {
             inst.opcode = value;
         })
        .def_property("rd",
         [](const DecodedInstruction<InstructionFormat::R_TYPE>& inst) {
             return inst.rd;
         },
         [](DecodedInstruction<InstructionFormat::R_TYPE>& inst, uint32_t value) {
             inst.opcode = value;
         })
        .def_property("funct3",
         [](const DecodedInstruction<InstructionFormat::R_TYPE>& inst) {
             return inst.funct3;
         },
         [](DecodedInstruction<InstructionFormat::R_TYPE>& inst, uint32_t value) {
             inst.funct3 = value;
         })
        .def_property("rs1",
         [](const DecodedInstruction<InstructionFormat::R_TYPE>& inst) {
             return inst.rs1;
         },
         [](DecodedInstruction<InstructionFormat::R_TYPE>& inst, uint32_t value) {
             inst.rs1 = value;
         })
        .def_property("rs2",
         [](const DecodedInstruction<InstructionFormat::R_TYPE>& inst) {
             return inst.rs2;
         },
         [](DecodedInstruction<InstructionFormat::R_TYPE>& inst, uint32_t value) {
             inst.rs2 = value;
         })
        .def_property("funct7",
         [](const DecodedInstruction<InstructionFormat::R_TYPE>& inst) {
             return inst.funct7;
         },
         [](DecodedInstruction<InstructionFormat::R_TYPE>& inst, uint32_t value) {
             inst.funct7 = value;
         })
        .def("get_opcode", &DecodedInstruction<InstructionFormat::R_TYPE>::get_opcode);

    //I type instructions
  py::class_<DecodedInstruction<InstructionFormat::I_TYPE>>(m, "DecodedInstructionIType")
        .def(py::init<uint32_t>())
        .def_property("opcode",
            [](const DecodedInstruction<InstructionFormat::I_TYPE>& inst) { return inst.opcode; },
            [](DecodedInstruction<InstructionFormat::I_TYPE>& inst, uint32_t val) { inst.opcode = val; })
        .def_property("rd",
            [](const DecodedInstruction<InstructionFormat::I_TYPE>& inst) { return inst.rd; },
            [](DecodedInstruction<InstructionFormat::I_TYPE>& inst, uint32_t val) { inst.rd = val; })
        .def_property("funct3",
            [](const DecodedInstruction<InstructionFormat::I_TYPE>& inst) { return inst.funct3; },
            [](DecodedInstruction<InstructionFormat::I_TYPE>& inst, uint32_t val) { inst.funct3 = val; })
        .def_property("rs1",
            [](const DecodedInstruction<InstructionFormat::I_TYPE>& inst) { return inst.rs1; },
            [](DecodedInstruction<InstructionFormat::I_TYPE>& inst, uint32_t val) { inst.rs1 = val; })
        .def_property("imm",
            [](const DecodedInstruction<InstructionFormat::I_TYPE>& inst) { return inst.imm; },
            [](DecodedInstruction<InstructionFormat::I_TYPE>& inst, int32_t val) { inst.imm = val; })
        .def("get_immediate", &DecodedInstruction<InstructionFormat::I_TYPE>::get_immediate)
        .def("get_opcode", &DecodedInstruction<InstructionFormat::I_TYPE>::get_opcode);

    // S-type instructions
    py::class_<DecodedInstruction<InstructionFormat::S_TYPE>>(m, "DecodedInstructionSType")
        .def(py::init<uint32_t>())
        .def_property("opcode",
            [](const DecodedInstruction<InstructionFormat::S_TYPE>& inst) { return inst.opcode; },
            [](DecodedInstruction<InstructionFormat::S_TYPE>& inst, uint32_t val) { inst.opcode = val; })
        .def_property("imm4_0",
            [](const DecodedInstruction<InstructionFormat::S_TYPE>& inst) { return inst.imm4_0; },
            [](DecodedInstruction<InstructionFormat::S_TYPE>& inst, uint32_t val) { inst.imm4_0 = val; })
        .def_property("funct3",
            [](const DecodedInstruction<InstructionFormat::S_TYPE>& inst) { return inst.funct3; },
            [](DecodedInstruction<InstructionFormat::S_TYPE>& inst, uint32_t val) { inst.funct3 = val; })
        .def_property("rs1",
            [](const DecodedInstruction<InstructionFormat::S_TYPE>& inst) { return inst.rs1; },
            [](DecodedInstruction<InstructionFormat::S_TYPE>& inst, uint32_t val) { inst.rs1 = val; })
        .def_property("rs2",
            [](const DecodedInstruction<InstructionFormat::S_TYPE>& inst) { return inst.rs2; },
            [](DecodedInstruction<InstructionFormat::S_TYPE>& inst, uint32_t val) { inst.rs2 = val; })
        .def_property("imm11_5",
            [](const DecodedInstruction<InstructionFormat::S_TYPE>& inst) { return inst.imm11_5; },
            [](DecodedInstruction<InstructionFormat::S_TYPE>& inst, uint32_t val) { inst.imm11_5 = val; })
        .def("get_immediate", &DecodedInstruction<InstructionFormat::S_TYPE>::get_immediate)
        .def("get_opcode", &DecodedInstruction<InstructionFormat::S_TYPE>::get_opcode);

    // B-type instructions
    py::class_<DecodedInstruction<InstructionFormat::B_TYPE>>(m, "DecodedInstructionBType")
        .def(py::init<uint32_t>())
        .def_property("opcode",
            [](const DecodedInstruction<InstructionFormat::B_TYPE>& inst) { return inst.opcode; },
            [](DecodedInstruction<InstructionFormat::B_TYPE>& inst, uint32_t val) { inst.opcode = val; })
        .def_property("imm11",
            [](const DecodedInstruction<InstructionFormat::B_TYPE>& inst) { return inst.imm11; },
            [](DecodedInstruction<InstructionFormat::B_TYPE>& inst, uint32_t val) { inst.imm11 = val; })
        .def_property("imm4_1",
            [](const DecodedInstruction<InstructionFormat::B_TYPE>& inst) { return inst.imm4_1; },
            [](DecodedInstruction<InstructionFormat::B_TYPE>& inst, uint32_t val) { inst.imm4_1 = val; })
        .def_property("funct3",
            [](const DecodedInstruction<InstructionFormat::B_TYPE>& inst) { return inst.funct3; },
            [](DecodedInstruction<InstructionFormat::B_TYPE>& inst, uint32_t val) { inst.funct3 = val; })
        .def_property("rs1",
            [](const DecodedInstruction<InstructionFormat::B_TYPE>& inst) { return inst.rs1; },
            [](DecodedInstruction<InstructionFormat::B_TYPE>& inst, uint32_t val) { inst.rs1 = val; })
        .def_property("rs2",
            [](const DecodedInstruction<InstructionFormat::B_TYPE>& inst) { return inst.rs2; },
            [](DecodedInstruction<InstructionFormat::B_TYPE>& inst, uint32_t val) { inst.rs2 = val; })
        .def_property("imm10_5",
            [](const DecodedInstruction<InstructionFormat::B_TYPE>& inst) { return inst.imm10_5; },
            [](DecodedInstruction<InstructionFormat::B_TYPE>& inst, uint32_t val) { inst.imm10_5 = val; })
        .def_property("imm12",
            [](const DecodedInstruction<InstructionFormat::B_TYPE>& inst) { return inst.imm12; },
            [](DecodedInstruction<InstructionFormat::B_TYPE>& inst, uint32_t val) { inst.imm12 = val; })
        .def("get_immediate", &DecodedInstruction<InstructionFormat::B_TYPE>::get_immediate)
        .def("get_opcode", &DecodedInstruction<InstructionFormat::B_TYPE>::get_opcode);

    // U-type instructions
    py::class_<DecodedInstruction<InstructionFormat::U_TYPE>>(m, "DecodedInstructionUType")
        .def(py::init<uint32_t>())
        .def_property("opcode",
            [](const DecodedInstruction<InstructionFormat::U_TYPE>& inst) { return inst.opcode; },
            [](DecodedInstruction<InstructionFormat::U_TYPE>& inst, uint32_t val) { inst.opcode = val; })
        .def_property("rd",
            [](const DecodedInstruction<InstructionFormat::U_TYPE>& inst) { return inst.rd; },
            [](DecodedInstruction<InstructionFormat::U_TYPE>& inst, uint32_t val) { inst.rd = val; })
        .def_property("imm",
            [](const DecodedInstruction<InstructionFormat::U_TYPE>& inst) { return inst.imm; },
            [](DecodedInstruction<InstructionFormat::U_TYPE>& inst, uint32_t val) { inst.imm = val; })
        .def("get_immediate", &DecodedInstruction<InstructionFormat::U_TYPE>::get_immediate)
        .def("get_opcode", &DecodedInstruction<InstructionFormat::U_TYPE>::get_opcode);

    // J-type instructions
    py::class_<DecodedInstruction<InstructionFormat::J_TYPE>>(m, "DecodedInstructionJType")
        .def(py::init<uint32_t>())
        .def_property("opcode",
            [](const DecodedInstruction<InstructionFormat::J_TYPE>& inst) { return inst.opcode; },
            [](DecodedInstruction<InstructionFormat::J_TYPE>& inst, uint32_t val) { inst.opcode = val; })
        .def_property("rd",
            [](const DecodedInstruction<InstructionFormat::J_TYPE>& inst) { return inst.rd; },
            [](DecodedInstruction<InstructionFormat::J_TYPE>& inst, uint32_t val) { inst.rd = val; })
        .def_property("imm19_12",
            [](const DecodedInstruction<InstructionFormat::J_TYPE>& inst) { return inst.imm19_12; },
            [](DecodedInstruction<InstructionFormat::J_TYPE>& inst, uint32_t val) { inst.imm19_12 = val; })
        .def_property("imm11",
            [](const DecodedInstruction<InstructionFormat::J_TYPE>& inst) { return inst.imm11; },
            [](DecodedInstruction<InstructionFormat::J_TYPE>& inst, uint32_t val) { inst.imm11 = val; })
        .def_property("imm10_1",
            [](const DecodedInstruction<InstructionFormat::J_TYPE>& inst) { return inst.imm10_1; },
            [](DecodedInstruction<InstructionFormat::J_TYPE>& inst, uint32_t val) { inst.imm10_1 = val; })
        .def_property("imm20",
            [](const DecodedInstruction<InstructionFormat::J_TYPE>& inst) { return inst.imm20; },
            [](DecodedInstruction<InstructionFormat::J_TYPE>& inst, uint32_t val) { inst.imm20 = val; })
        .def("get_immediate", &DecodedInstruction<InstructionFormat::J_TYPE>::get_immediate)
        .def("get_opcode", &DecodedInstruction<InstructionFormat::J_TYPE>::get_opcode);
}
