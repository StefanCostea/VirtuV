#pragma once
#include <array>
#include <cstdint>

class CPU {
private:
    std::array<uint32_t, 32> registers; // 32 General-purpose registers
    uint32_t pc;                        // Program Counter

public:
    CPU();
    uint32_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint32_t value);
    uint32_t getPC();           
    void setPC(uint32_t value);
};
