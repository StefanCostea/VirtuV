#include "RegisterBank.hpp"

RegisterBank::RegisterBank() {
    registers.fill(0); // Initialize all registers to 0
}

uint32_t RegisterBank::read(uint8_t reg) {
    if (reg >= registers.size()) {
        throw std::out_of_range("Register index out of range");
    }
    return registers[reg];
}

void RegisterBank::write(uint8_t reg, uint32_t value) {
    if (reg == 0) {
        throw std::invalid_argument("Cannot write to x0 (read-only register)");
    }
    if (reg >= registers.size()) {
        throw std::out_of_range("Register index out of range");
    }
    registers[reg] = value;
}
