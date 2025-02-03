#pragma once
#include "core/cpu/register_bank/RegisterBank.hpp"
#include "core/cpu/pipeline/execute/ExecuteStage.hpp"
#include "core/cpu/pipeline/memory_access/MemoryAccessStage.hpp"    
#include <cstdint>

class WriteBackStage {
public:
    WriteBackStage(RegisterBank& register_bank);

    void set_execution_result(const ExecutionResult& exec_result);
    void set_memory_access_result(const MemoryAccessResult& mem_result);
    void set_decoded_instruction(const DecodedInstructionVariant& decoded_instr);

    void process();

private:
    RegisterBank& register_bank;
    ExecutionResult execution_result;
    MemoryAccessResult memory_access_result;
    DecodedInstructionVariant decoded_instruction;
};

