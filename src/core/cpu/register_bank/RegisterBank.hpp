#pragma once
#include <array>
//#include <cstdint>
#include <stdexcept>

class RegisterBank {
private:
    std::array<uint32_t, 32> registers{}; // General-purpose registers

public:
    RegisterBank();

    uint32_t read(uint8_t reg);
    void write(uint8_t reg, uint32_t value);
};
