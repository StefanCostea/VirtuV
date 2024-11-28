#include "cpu/cpu.hpp"

#include <stdexcept>

CPU::CPU() : registers{}, pc(0) {
    // Initialize all registers to 0
    registers.fill(0);
}

uint32_t CPU::read_register(uint8_t reg) const {
    if (reg >= registers.size()) {
        throw std::out_of_range("Register index out of range");
    }
    return registers[reg];
}

void CPU::write_register(uint8_t reg, uint32_t value) {
    if (reg == 0) {
        throw std::invalid_argument("Invalid write to x0: Register x0 is read-only");
    }
    if (reg >= registers.size()) {
        throw std::out_of_range("Register index out of range");
    }
    registers[reg] = value;
}

uint32_t CPU::get_pc() const {
    return pc;
}

void CPU::set_pc(uint32_t value) {
    pc = value;
}
