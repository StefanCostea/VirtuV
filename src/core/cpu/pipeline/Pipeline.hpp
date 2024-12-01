#pragma once
#include "core/cpu/register_bank/RegisterBank.hpp"
#include "core/memory/MMU.hpp"
#include "decode/DecodeStage.hpp"
#include "fetch/FetchStage.hpp"

class Pipeline {
private:
    RegisterBank& register_bank;
    MMU& mmu;

    FetchStage fetch_stage;
    DecodeStage decode_stage;
public:
    Pipeline(RegisterBank& register_bank, MMU& mmu);

    void run_cycle();
};
