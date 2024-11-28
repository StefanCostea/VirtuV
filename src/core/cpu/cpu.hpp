#pragma once
#include <array>
#include <cstdint>

class CPU {
private:
    std::array<uint32_t, 32> registers; // 32 General-purpose registers
    uint32_t pc;                        // Program Counter

public:
    CPU();
    uint32_t read_register(uint8_t reg);
    void write_register(uint8_t reg, uint32_t value);
    uint32_t get_pc();           
    void set_pc(uint32_t value);
};
