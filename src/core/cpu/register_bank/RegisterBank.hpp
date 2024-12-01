#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>

class RegisterBank {
private:
    std::array<uint32_t, 32> registers{}; // General-purpose registers
    uint32_t pc;
public:
    RegisterBank();

    uint32_t read(uint8_t reg) const;
    void write(uint8_t reg, uint32_t value);

    /**
     * @brief Gets the current value of the program counter (PC).
     * @return The value of the PC.
     */
    uint32_t get_pc() const;

    /**
     * @brief Sets the value of the program counter (PC).
     * @param value The new value for the PC.
     */
    void set_pc(uint32_t value);
};
