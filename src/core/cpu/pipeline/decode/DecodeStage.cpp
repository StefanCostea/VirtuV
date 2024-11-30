#include "DecodeStage.hpp"
#include <stdexcept>
#include <cstdint>

// Constructor
DecodeStage::DecodeStage(RegisterBank& register_bank)
    : fetched_instruction(0), register_bank(register_bank), decoded_instruction(DecodedInstruction<InstructionFormat::INIVALID_TYPE>(0)){}

// Process the fetched instruction and decode it
void DecodeStage::process() {
    // Extract the opcode from the fetched instruction
    uint32_t opcode = DecodedInstructionBase(fetched_instruction).get_opcode();

    InstructionFormat format = static_cast<InstructionFormat>(opcode);
    // Decode the instruction based on the opcode
    using enum InstructionFormat;

    switch (format) {
        case R_TYPE:
            decoded_instruction = DecodedInstruction<R_TYPE>(fetched_instruction);
            break;
        case I_TYPE:
            decoded_instruction = DecodedInstruction<I_TYPE>(fetched_instruction);
            break;
        case S_TYPE:
            decoded_instruction = DecodedInstruction<S_TYPE>(fetched_instruction);
            break;
        case B_TYPE:
            decoded_instruction = DecodedInstruction<B_TYPE>(fetched_instruction);
            break;
        case U_TYPE:
            decoded_instruction = DecodedInstruction<U_TYPE>(fetched_instruction);
            break;
        case J_TYPE:
            decoded_instruction = DecodedInstruction<J_TYPE>(fetched_instruction);
            break;
        default:
            throw std::invalid_argument("Unsupported instruction format");
    }
}

// Get the decoded instruction (This is the output)
const DecodedInstructionVariant& DecodeStage::get_decoded_instruction() const {
    return decoded_instruction;
}

// Set the fetched instruction (from FetchStage output)
void DecodeStage::set_fetched_instruction(uint32_t instruction) {
    fetched_instruction = instruction;
}
