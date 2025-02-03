#include "CPU.hpp"
#include <fstream>
#include <iostream>
#include <vector>

CPU::CPU(size_t memory_size)
    : register_bank(),                           
      physical_memory(memory_size),                
      page_table(),                                
      mmu(&physical_memory, &page_table, PrivilegeMode::MACHINE), 
      pipeline(register_bank, mmu),              
      privilege_mode(PrivilegeMode::MACHINE)      
{
}

int CPU::load_program(const std::string &filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open program file: " << filepath << std::endl;
        return -1;
    }
    // get size of program
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // allocate buffer and copy program to it
    std::vector<char> buffer(static_cast<size_t>(size));
    if (!file.read(buffer.data(), size)) {
        std::cerr << "Error: Unable to read program file: " << filepath << std::endl;
        return -1;
    }
    file.close();

    //For the moment copy the program to memory starting from address 0
    for (std::streamsize i = 0; i < size; ++i) {
        try {
            mmu.write(static_cast<uint32_t>(i), static_cast<uint8_t>(buffer[i]));
        } catch (const std::exception& e) {
            std::cerr << "Error writing to physical memory at address " << i << ": " << e.what() << std::endl;
            return -1;
        }
    }
    //set initial pc where the program starts
    register_bank.set_pc(0);

    std::cout << "Program loaded successfully (" << size << " bytes)." << std::endl;
    return 0;
}

void CPU::run() {
    try {
        while (true) {
            pipeline.run_cycle();
        }
    } catch (const std::exception& e) {
        std::cerr << "CPU Exception: " << e.what() << std::endl;
    }
}

uint32_t CPU::get_register(uint8_t reg){
    return register_bank.read(reg);
}