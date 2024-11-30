#pragma once

#include <variant>
#include "core/cpu/pipeline/PipelineStage.hpp"
#include "core/cpu/register_bank/RegisterBank.hpp"
#include "core/cpu/isa/Instruction.hpp"

class DecodeStage : public PipelineStage {
private:
    uint32_t fetched_instruction;                   // Instruction fetched in FetchStage
    RegisterBank& register_bank;                    // Reference to the register bank
    DecodedInstructionVariant decoded_instruction;  // Decoded instruction

public:
    DecodeStage(RegisterBank& register_bank);

    void process() override;

    const DecodedInstructionVariant& get_decoded_instruction() const;

    void set_fetched_instruction(uint32_t fetched_instruction);
};