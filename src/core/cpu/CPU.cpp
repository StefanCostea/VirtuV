#include "CPU.hpp"

CPU::CPU(size_t memory_size)
    : register_bank(), memory(memmemory_sizeorySize), pipeline(&register_bank, &memory) {}

void CPU::load_program(const std::string& filepath) {
    return 0; //TODO: Implement load program to memory
}

void CPU::run() {
    while (true) {
        pipeline.run_cycle();
    }
}
