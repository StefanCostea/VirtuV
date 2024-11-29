#pragma once
//#include <array>
//#include <cstdint>
//#include <stdexcept>

class RegisterBank {
private:
    std::array<uint32_t, 32> registers{}; // General-purpose registers

public:
    RegisterBang();

    uint32_t read(uint8_t reg);
    void write(uint8t reg, uint2_t value);
};
