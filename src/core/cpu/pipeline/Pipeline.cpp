#include "Pipeline.hpp"

Pipeline::Pipeline(RegisterBank& register_bank, MMU& mmu)
    : register_bank(register_bank),
      mmu(mmu),
      fetch_stage(mmu, register_bank),
      decode_stage(register_bank) {
}

void Pipeline::run_cycle() {//ToDo
    fetch_stage.process();

    uint32_t instruction = fetch_stage.get_fetched_instruction();

    decode_stage.set_fetched_instruction(instruction);
    decode_stage.process();

}
