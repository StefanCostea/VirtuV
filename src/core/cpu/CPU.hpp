#pragma once

#include "Pipeline.hpp"
#include "RegisterBank.hpp"
#include "Memory.hpp"

class CPU {
private:
    RegisterBank register_bank; // Manages registers
    Memory memory;              // Manages memory
    Pipeline pipeline;          // Manages instruction processing

public:
    CPU(size_t memory_size);

    void load_program(const std::string& filepath); // Load a binary program
    void run();                                     // Run the CPU
};
