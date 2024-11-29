#pragma once
#include "core/cpu/CPU.hpp"
#include "core/cpu/register_bank/RegisterBank.hpp"
#include "core/memory/Memory.hpp"

class Pipeline {
private:
    RegisterBank* registerBank;
    Memory* memory;
public:
    void run_cycle();

    Pipeline() = default;
    Pipeline(RegisterBank* register_bank, Memory* memory);

};
