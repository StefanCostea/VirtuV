#pragma once
#include <cstdint>
#include "core/cpu/pipeline/PipelineStage.hpp"
#include "core/cpu/register_bank/RegisterBank.hpp"
#include "core/memory/MMU.hpp"

class FetchStage : public PipelineStage {
private:
    MMU& mmu;
    uint32_t fetched_instruction;
    RegisterBank& register_bank;
public:
    FetchStage(MMU& mmu, RegisterBank& register_bank);
    void process() override;
    uint32_t get_fetched_instruction();
};
