#pragma once

#include "core/cpu/register_bank/RegisterBank.hpp"
#include "core/cpu/pipeline/PipelineStage.hpp"
#include "core/cpu/isa/Instruction.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <exception>

// Execution result structure
struct ExecutionResult {
    uint32_t alu_result;     
    bool branch_taken = false;
    uint32_t branch_target = 0;
};

//Exception thrown when jump to self is detected such that top level can detect end of program and stop the execution gracefully
class EndOfProgramException : public std::exception {
    public:
        explicit EndOfProgramException(const std::string& msg = "End of program reached")
            : message(msg) {}
    
        virtual const char* what() const noexcept override {
            return message.c_str();
        }
        
    private:
        std::string message;
};

class ExecuteStage : public PipelineStage {
private:
    const RegisterBank& register_bank;
    DecodedInstructionVariant decoded_instruction; // Input decoded instruction
    uint32_t rs1_value;                             // Value from register rs1
    uint32_t rs2_value;                             // Value from register rs2
    int32_t immediate;                              // Immediate value
    ExecutionResult result;                         // Execution result

public:
    ExecuteStage(RegisterBank& register_bank);

    void process() override;

    const ExecutionResult& get_result() const;

    void set_decoded_instruction(const DecodedInstructionVariant& instruction);
};
