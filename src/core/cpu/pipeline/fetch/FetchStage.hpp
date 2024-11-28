#pragma once
#include "PipelineStage.hpp"
#include "Memory.hpp"
#include "RegisterBank.hpp"

class FetchStage : public PipelineStage {
private:
    Memory* memory;
    RegisterBank* cpu;
    uint32_t fetched_instruction;

public:
    FetchStage(Memory* memory, CPU* cpu);
    void process() override;
    uint32_t get_fetched_instruction();
};
