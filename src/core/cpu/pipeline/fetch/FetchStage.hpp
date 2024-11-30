#pragma once
#include <cstdint>
#include "core/cpu/pipeline/PipelineStage.hpp"
#include "core/memory/Memory.hpp"

class FetchStage : public PipelineStage {
private:
    Memory& memory;
    uint32_t& program_counter;
    uint32_t fetched_instruction;

public:
    FetchStage(Memory& memory, uint32_t& program_counter);
    void process() override;
    uint32_t get_fetched_instruction();
};
