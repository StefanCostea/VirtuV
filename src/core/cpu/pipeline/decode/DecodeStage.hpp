#pragma once

#include <variant>
#include "core/cpu/pipeline/PipelineStage.hpp"
#include "core/cpu/register_bank/RegisterBank.hpp"
#include "core/cpu/isa/Instruction.hpp"

// Define the variant type to hold all possible instruction formats
using DecodedInstructionVariant = std::variant<
    DecodedInstruction<InstructionFormat::INIVALID_TYPE>,
    DecodedInstruction<InstructionFormat::R_TYPE>,
    DecodedInstruction<InstructionFormat::I_TYPE>,
    DecodedInstruction<InstructionFormat::S_TYPE>,
    DecodedInstruction<InstructionFormat::B_TYPE>,
    DecodedInstruction<InstructionFormat::U_TYPE>,
    DecodedInstruction<InstructionFormat::J_TYPE>
>;

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