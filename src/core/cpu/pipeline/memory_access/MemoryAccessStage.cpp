#include "MemoryAccessStage.hpp"
#include <stdexcept>

MemoryAccessStage::MemoryAccessStage(MMU& mmu, RegisterBank& register_bank)
    : mmu(mmu), register_bank(register_bank), decoded_instruction(DecodedInstruction<InstructionFormat::INIVALID_TYPE>(0))
{
    result.load_data.reset(); //reset result
    result.store_success = false;
}

void MemoryAccessStage::set_execution_result(const ExecutionResult& exec_result) {
    execution_result = exec_result;
}

void MemoryAccessStage::process() {
    result.load_data.reset(); // reset
    result.store_success = false;

    uint32_t effective_address = execution_result.alu_result;

    //decay the variant of the decoded instruction
    std::visit([this, effective_address](auto& inst) {
        using T = std::decay_t<decltype(inst)>;

        // if I format it could be LOAD 
        if constexpr (T::format == InstructionFormat::I_TYPE) {
            if (inst.opcode == 0x03) { //TODO: implement different than word
                result.load_data = mmu.read_word(effective_address);
            }
        }
        // else type S is store
        else if constexpr (T::format == InstructionFormat::S_TYPE) { //TODO: Implement different than word
            auto& s_inst = std::get<DecodedInstruction<InstructionFormat::S_TYPE>>(decoded_instruction);
            uint32_t rs2 = s_inst.rs2;
            uint32_t data_to_store = register_bank.read(rs2);
            mmu.write_word(effective_address, data_to_store);
            result.store_success = true;
        }
    }, decoded_instruction);
}

const MemoryAccessResult& MemoryAccessStage::get_result() const {
    return result;
}

//Set input decoded instruction
void MemoryAccessStage::set_decoded_instruction(const DecodedInstructionVariant& instruction) {
    decoded_instruction = instruction;   
}
