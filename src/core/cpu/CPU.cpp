#include "CPU.hpp"
#include <iostream>

CPU::CPU(size_t memory_size)
: pipeline(register_bank, mmu),
  mmu(&physical_memory, &page_table, PrivilegeMode::MACHINE),
  physical_memory(memory_size),
  privilege_mode(PrivilegeMode::MACHINE) {}

int CPU::load_program(const std::string &filepath) {
    return 0; //TODO: Implement load program to memory
}

void CPU::run() {
    try {
        while (true) {
            pipeline.run_cycle();
        }
    } catch (const std::exception& e) {
        // Handle exceptions, such as memory access violations
        // For now, we simply print the error and exit
        std::cerr << "CPU Exception: " << e.what() << std::endl;
    }
}
