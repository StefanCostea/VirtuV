#include "FetchStage.hpp"

FetchStage::FetchStage(Memory& memory, uint32_t& program_counter)
    : memory(memory), program_counter(program_counter), fetched_instruction(0) {}

void FetchStage::process() {
    // Fetch the instruction from memory at the current program counter
    fetched_instruction = memory.read(program_counter);

    // Increment the program counter to point to the next instruction
    program_counter += 4;
}

uint32_t FetchStage::get_fetched_instruction() {
    return fetched_instruction;
}