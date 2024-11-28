#pragma once

#include "pipeline/Pipeline.hpp"
#include "register_bank/RegisterBank.hpp"
#include "memory/Memory.hpp"

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
