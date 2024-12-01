#pragma once
#include <vector>
#include <cstdint>
/**
 * @brief Represents the physical memory of the system.
 *
 * This class simulates the physical memory (RAM) where data and instructions are stored.
 */
class PhysicalMemory {
private:
    std::vector<uint8_t> memory; /**< The memory storage represented as a byte array */

public:
    /**
     * @brief Constructs a PhysicalMemory object with a given size.
     * @param size The size of the memory in bytes.
     */
    explicit PhysicalMemory(size_t size);
    /**
     * @brief Reads a byte from a physical memory address.
     * @param address The physical address to read from.
     * @return The byte value at the specified address.
     * @throws std::out_of_range if the address is out of bounds.
     */
    uint8_t read(uint32_t address);
    /**
     * @brief Writes a byte to a physical memory address.
     * @param address The physical address to write to.
     * @param value The byte value to write.
     * @throws std::out_of_range if the address is out of bounds.
     */
    void write(uint32_t address, uint8_t value);
};
