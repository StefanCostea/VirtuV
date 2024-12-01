#pragma once

#include <string>

#include "core/cpu/pipeline/Pipeline.hpp"
#include "core/cpu/register_bank/RegisterBank.hpp"
#include "core/memory/MMU.hpp"

class CPU {
private:
    RegisterBank register_bank;     // Manages registers
    Pipeline pipeline;              // Manages instruction processing
    MMU mmu;                        /**< Memory Management Unit */
    PhysicalMemory physical_memory;
    PageTable page_table;
    PrivilegeMode privilege_mode;   /**< Current privilege mode of the CPU */

public:
    CPU(size_t memory_size);

    int load_program(const std::string &filepath); // Load a binary program
    void run();                                     // Run the CPU
};
