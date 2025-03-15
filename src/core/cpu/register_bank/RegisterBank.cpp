#include "RegisterBank.hpp"
#include "utils/plt.hpp"

RegisterBank::RegisterBank() : pc(0) {
    registers.fill(0); // Initialize all registers to 0
}

uint32_t RegisterBank::read(uint8_t reg) const {
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

uint32_t RegisterBank::get_pc() const {
    return pc;
}

void RegisterBank::set_pc(uint32_t value) {
    pc = value;
}
