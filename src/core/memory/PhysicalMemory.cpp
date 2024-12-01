#include "PhysicalMemory.hpp"
#include <stdexcept>

PhysicalMemory::PhysicalMemory(size_t size) : memory(size, 0) {}

uint8_t PhysicalMemory::read(uint32_t address) {
    if (address >= memory.size()) {
        throw std::out_of_range("PhysicalMemory::read - Address out of range");
    }
    return memory[address];
}

void PhysicalMemory::write(uint32_t address, uint8_t value) {
    if (address >= memory.size()) {
        throw std::out_of_range("PhysicalMemory::write - Address out of range");
    }
    memory[address] = value;
}
