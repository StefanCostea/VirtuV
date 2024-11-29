#pragma once
#include "core/cpu/pipeline/PipelineStage.hpp"
#include "core/memory/Memory.hpp"
#include "core/cpu/register_bank/RegisterBank.hpp"

class FetchStage : public PipelineStage {
private:
    Memory* memory;
    RegisterBank* registers;
    uint32_t fetched_instruction;

public:
    FetchStage(Memory* memory, RegisterBank* registers);
    void process() override;
    uint32_t get_fetched_instruction();
};
