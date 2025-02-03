#include "Pipeline.hpp"

Pipeline::Pipeline(RegisterBank& register_bank, MMU& mmu)
    : register_bank(register_bank),
      mmu(mmu),
      fetch_stage(mmu, register_bank),
      decode_stage(register_bank),
      execute_stage(register_bank),
      mem_acces_stage(mmu, register_bank),
      write_back_stage(register_bank)
{
}

void Pipeline::run_cycle() {
    // --- Fetch Stage ---
    fetch_stage.process();
    uint32_t instruction = fetch_stage.get_fetched_instruction();

    // --- Decode Stage ---
    decode_stage.set_fetched_instruction(instruction);
    decode_stage.process();
    // Retrieve the decoded instruction variant for the next stages.
    auto decoded_inst = decode_stage.get_decoded_instruction();

    // --- Execute Stage ---
    execute_stage.set_decoded_instruction(decoded_inst);
    execute_stage.process();
    auto exec_result = execute_stage.get_result();

    // --- Memory Access Stage ---
    mem_acces_stage.set_execution_result(exec_result);
    mem_acces_stage.set_decoded_instruction(decoded_inst);
    mem_acces_stage.process();
    auto mem_result = mem_acces_stage.get_result();

    // --- Write Back Stage ---
    write_back_stage.set_execution_result(exec_result);
    write_back_stage.set_memory_access_result(mem_result);
    write_back_stage.set_decoded_instruction(decoded_inst);
    write_back_stage.process();
}
