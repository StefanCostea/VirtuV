#include "WriteBackStage.hpp"
#include <stdexcept>
#include <variant>

WriteBackStage::WriteBackStage(RegisterBank& register_bank)
    : register_bank(register_bank), decoded_instruction(DecodedInstruction<InstructionFormat::INIVALID_TYPE>(0))
{
}

void WriteBackStage::set_execution_result(const ExecutionResult& exec_result) {
    execution_result = exec_result;
}

void WriteBackStage::set_memory_access_result(const MemoryAccessResult& mem_result) {
    memory_access_result = mem_result;
}

void WriteBackStage::set_decoded_instruction(const DecodedInstructionVariant& decoded_instr) {
    decoded_instruction = decoded_instr;
}

void WriteBackStage::process() {
    //Use visit to decay instruction variant
    std::visit([this](auto& inst) {
        using T = std::decay_t<decltype(inst)>;

        if constexpr (T::format == InstructionFormat::R_TYPE) {
            //Write in rd
            if (inst.rd != 0) {
                register_bank.write(inst.rd, execution_result.alu_result);
            }
        }
        else if constexpr (T::format == InstructionFormat::I_TYPE) {
            //could be load or arithmetic instruction
            if (inst.opcode == 0x03) { // load
                if (inst.rd != 0 && memory_access_result.load_data.has_value()) {
                    register_bank.write(inst.rd, memory_access_result.load_data.value());
                }
            } else {
                // arithmetic instruction write in rd
                if (inst.rd != 0) {
                    register_bank.write(inst.rd, execution_result.alu_result);
                }
            }
        }
        else if constexpr (T::format == InstructionFormat::U_TYPE) {
            //write in rd
            if (inst.rd != 0) {
                register_bank.write(inst.rd, execution_result.alu_result);
            }
        }
        else if constexpr (T::format == InstructionFormat::J_TYPE) {
            if (inst.rd != 0) {
                register_bank.write(inst.rd, execution_result.alu_result);
            }
        }
        // s type and b type do not need WB
    }, decoded_instruction);
}
