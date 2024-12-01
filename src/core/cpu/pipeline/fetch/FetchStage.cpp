#include "FetchStage.hpp"

FetchStage::FetchStage(MMU& mmu, RegisterBank& register_bank)
    : mmu(mmu),  fetched_instruction(0), register_bank(register_bank) {}

void FetchStage::process() {
    // Fetch the instruction from memory at the current program counter
    uint32_t pc = register_bank.get_pc();
    fetched_instruction = mmu.read_word(pc);

    // Increment the program counter to point to the next instruction
    register_bank.set_pc(pc + 4);
}

uint32_t FetchStage::get_fetched_instruction() {
    return fetched_instruction;
}