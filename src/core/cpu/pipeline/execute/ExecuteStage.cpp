#include "ExecuteStage.hpp"
#include <stdexcept>
#include <variant>
#include "utils/plt.hpp"

// Constructor
ExecuteStage::ExecuteStage(RegisterBank& register_bank)
    : register_bank(register_bank), decoded_instruction(DecodedInstruction<InstructionFormat::INIVALID_TYPE>(0)), rs1_value(0), rs2_value(0), immediate(0) {}

// Process the instruction
void ExecuteStage::process() {
    if (!decoded_instruction.index()) { // Check if the variant is not initialized
        throw std::runtime_error("Decoded instruction is not set for execution");
    }

    std::visit(
        [this](auto& instruction) {
            using T = std::decay_t<decltype(instruction)>;

            // Handle R-Type Instructions
            if constexpr (T::format == InstructionFormat::R_TYPE) {
                rs1_value = register_bank.read(instruction.rs1);
                rs2_value = register_bank.read(instruction.rs2);

                switch (instruction.funct3) {
                    case 0x0: // ADD or SUB
                        result.alu_result = (instruction.funct7 == 0x20)
                                                ? rs1_value - rs2_value  // SUB
                                                : rs1_value + rs2_value; // ADD
                        break;
                    default:
                        throw std::invalid_argument("Unsupported R-Type funct3");
                }
            }

            // Handle I-Type Instructions
            else if constexpr (T::format == InstructionFormat::I_TYPE) {
                rs1_value = register_bank.read(instruction.rs1);
                immediate = instruction.get_immediate();

                switch (instruction.funct3) {
                    case 0x0: // ADDI
                        result.alu_result = rs1_value + immediate;
                        break;
                    case 0x2: // SLTI: Set less than (signed)
                        result.alu_result = (static_cast<int32_t>(rs1_value) < immediate) ? 1 : 0;
                        break;
                    case 0x3: // SLTIU: Set less than (unsigned)
                        result.alu_result = (rs1_value < static_cast<uint32_t>(immediate)) ? 1 : 0;
                        break;
                    case 0x4: // XORI
                        result.alu_result = rs1_value ^ immediate;
                        break;
                    case 0x6: // ORI
                        result.alu_result = rs1_value | immediate;
                        break;
                    case 0x7: // ANDI
                        result.alu_result = rs1_value & immediate;
                        break;
                    case 0x1: { // SLLI (Shift Left Logical Immediate)
                        // For SLLI, the lower 5 bits of the immediate specify the shift amount.
                        uint32_t shamt = immediate & 0x1F;
                        result.alu_result = rs1_value << shamt;
                        break;
                    }
                    case 0x5: { // Either SRLI (logical shift right)
                        uint32_t shamt = immediate & 0x1F;
                        // Determine shift type based on funct7 field (which is part of the instruction encoding)
                        result.alu_result = rs1_value >> shamt;
                        break;
                    }
                    default:
                        throw std::invalid_argument("Unsupported I-Type funct3");
                }
            }

            // Handle B-Type Instructions
            else if constexpr (T::format == InstructionFormat::B_TYPE) {
                rs1_value = register_bank.read(instruction.rs1);
                rs2_value = register_bank.read(instruction.rs2);
                immediate = instruction.get_immediate();

                switch (instruction.funct3) {
                    case 0x0: // BEQ
                        result.branch_taken = (rs1_value == rs2_value);
                        result.branch_target = result.branch_taken ? immediate : 0;
                        break;
                    case 0x1: // BNE
                        result.branch_taken = (rs1_value != rs2_value);
                        result.branch_target = result.branch_taken ? immediate : 0;
                        break;
                    default:
                        throw std::invalid_argument("Unsupported B-Type funct3");
                }
            }

            // Handle S-Type Instructions
            else if constexpr (T::format == InstructionFormat::S_TYPE) {
                rs1_value = register_bank.read(instruction.rs1);
                rs2_value = register_bank.read(instruction.rs2);
                immediate = instruction.get_immediate();

                result.alu_result = rs1_value + immediate; 
            }

            // Handle U-Type Instructions
            else if constexpr (T::format == InstructionFormat::U_TYPE) {
                immediate = instruction.get_immediate();

                result.alu_result = immediate; 
            }

            // Handle J-Type Instructions
            else if constexpr (T::format == InstructionFormat::J_TYPE) {
                immediate = instruction.get_immediate();

                result.branch_taken = true;
                result.branch_target = register_bank.get_pc() + immediate;

                //Handle Jump to Self instruction (Infinite loop detection to determine end of program)
                if (result.branch_target == register_bank.get_pc()) {
                    throw EndOfProgramException();
                }
            }

            // Unsupported instruction format
            else {
                throw std::invalid_argument("Unsupported instruction format for execution stage");
            }
        },
        decoded_instruction
    );
}

// Get the execution result
const ExecutionResult& ExecuteStage::get_result() const {
    return result;
}

//Set input decoded instruction
void ExecuteStage::set_decoded_instruction(const DecodedInstructionVariant& instruction) {
    decoded_instruction = instruction;   
}
