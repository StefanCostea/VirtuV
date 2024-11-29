#pragma once

#include "core/cpu/pipeline/Pipeline.hpp"
#include "core/cpu/register_bank/RegisterBank.hpp"
#include "core/memory/Memory.hpp"

class CPU {
private:
    RegisteBank register_bank; // Manages registers
    Memory memory;              // Manages memory
    Pipeline pipeline;          // Manages instruction processing

public:
    CPU(size_t memory_size);

    int load_program(const std::string &filepath); // Load a binary program
    void run();                                     // Run the CPU
};
