#include "CPU.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include "utils/plt.hpp"

CPU::CPU(size_t memory_size)
    : register_bank(),                           
      physical_memory(memory_size),                
      page_table(),                                
      mmu(&physical_memory, &page_table, PrivilegeMode::MACHINE), 
      pipeline(register_bank, mmu),              
      privilege_mode(PrivilegeMode::MACHINE)      
{
    uint32_t virtual_address = 0x0000;
    uint32_t page_number = virtual_address & 0xFFFFF000;
    uint32_t entry_value = (page_number & 0xFFFFF000)
                           | PageTableEntry::VALID_BIT | PageTableEntry::READ_BIT | PageTableEntry::WRITE_BIT | PageTableEntry::EXECUTE_BIT | PageTableEntry::USER_ACCESSIBLE_BIT;
    page_table.add_entry(page_number, PageTableEntry(entry_value));
}

int CPU::load_program(const std::string &filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        PLT_ERROR("Error: Unable to open program file: " + filepath);
        return -1;
    }
    // get size of program
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // allocate buffer and copy program to it
    std::vector<char> buffer(static_cast<size_t>(size));
    if (!file.read(buffer.data(), size)) {
        PLT_ERROR("Error: Unable to read program file: " + filepath);
        return -1;
    }
    file.close();

    //For the moment copy the program to memory starting from address 0
    for (std::streamsize i = 0; i < size; ++i) {
        try {
            mmu.write(static_cast<uint32_t>(i), static_cast<uint8_t>(buffer[i]));
        } catch (const std::exception& e) {
            PLT_ERROR("Error writing to physical memory at address " + std::to_string(i) + ": " + e.what());
            return -1;
        }
    }
    //set initial pc where the program starts
    register_bank.set_pc(0);

    PLT_INFO("Program loaded successfully (" + std::to_string(size) + " bytes).");
    return 0;
}

void CPU::run() {
    try {
        while (true) {
            PLT_DEBUG("INSTRUCTION");
            pipeline.run_cycle();
        }
    } catch (const EndOfProgramException& e){
        PLT_INFO("CPU ended program, exiting simulation");
    } catch (const std::exception& e) {
        PLT_ERROR("CPU Exception: " + std::string(e.what()));
        throw e;
    }
}

uint32_t CPU::get_register(uint8_t reg){
    return register_bank.read(reg);
}