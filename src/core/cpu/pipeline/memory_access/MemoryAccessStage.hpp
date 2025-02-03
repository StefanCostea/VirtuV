#pragma once

#include "core/memory/MMU.hpp"
#include "core/cpu/pipeline/execute/ExecuteStage.hpp"
#include <cstdint>
#include <optional>

struct MemoryAccessResult {
    std::optional<uint32_t> load_data;  // read value in case of load
    bool store_success = false;        
};

class MemoryAccessStage {
private:
    MMU& mmu;
    const RegisterBank& register_bank;
    ExecutionResult execution_result;
    DecodedInstructionVariant decoded_instruction; // Input decoded instruction
    MemoryAccessResult result;
    
public:
    MemoryAccessStage(MMU& mmu, RegisterBank& register_bank);

    void set_execution_result(const ExecutionResult& exec_result);
    void set_decoded_instruction(const DecodedInstructionVariant& instruction);

    void process();

    const MemoryAccessResult& get_result() const;

};

