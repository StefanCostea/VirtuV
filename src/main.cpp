#include <iostream>

#include "core/cpu/CPU.hpp"

int main(int argc, char* argv[]) {
    // CPU instance with 1 MB of memory
    CPU cpu(1024 * 1024);

    if (argc < 2) {
        std::cerr << "Usage: virtuv <program[.bin]>" << std::endl;
        return 1;
    }

    if (cpu.load_program(argv[1]) != 0) {
        std::cerr << "Failed to load program: " << argv[1] << std::endl;
        return 1;
    }

    cpu.run();

    return 0;
}
