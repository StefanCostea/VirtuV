#pragma once
#include <cstdint>
#include <cstddef>

class Memory {
private:

public:
    explicit Memory(size_t size);

    uint32_t read(uint32_t address) const;
};
