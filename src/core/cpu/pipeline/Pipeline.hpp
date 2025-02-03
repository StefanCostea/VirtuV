#pragma once
#include "core/cpu/register_bank/RegisterBank.hpp"
#include "core/memory/MMU.hpp"
#include "decode/DecodeStage.hpp"
#include "fetch/FetchStage.hpp"
#include "execute/ExecuteStage.hpp"
#include "memory_access/MemoryAccessStage.hpp"
#include "write_back/WriteBackStage.hpp"

class Pipeline {
private:
    RegisterBank& register_bank;
    MMU& mmu;

    FetchStage fetch_stage;
    DecodeStage decode_stage;
    ExecuteStage execute_stage;
    MemoryAccessStage mem_acces_stage;
    WriteBackStage write_back_stage;
public:
    Pipeline(RegisterBank& register_bank, MMU& mmu);

    void run_cycle();
};
