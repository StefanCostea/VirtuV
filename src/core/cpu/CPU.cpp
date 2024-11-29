#include "CPU.hpp"

CPU::CPU(size_t memory_size)
    : register_bank(), memory(memory_size), pipeline(&register_bank, &memory) {}

int CPU::load_program(const std::string &filepath) {
    return 0; //TODO: Implement load program to memory
}

void CPU::run() {
    while (true) {
        pipeline.run_cycle();
    }
}
