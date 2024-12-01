#include "MMU.hpp"
#include <stdexcept>
MMU::MMU(PhysicalMemory* phys_mem, PageTable* pt, PrivilegeMode mode)
    : physical_memory(phys_mem), page_table(pt), privilege_mode(mode) {}

uint32_t MMU::translate_address(uint32_t virtual_address, bool is_write) {
    // 4KB pages and direct mapping
    uint32_t page_number = virtual_address & 0xFFFFF000;

    PageTableEntry entry = page_table->get_entry(page_number);

    if (!entry.is_valid()) {
        throw PageFaultException("MMU::translate_address - Invalid page entry");
    }

    if (is_write && !entry.is_writable(privilege_mode)) {
        throw AccessViolationException("MMU::translate_address - Write not permitted on this page");
    }

    if (!is_write && !entry.is_readable(privilege_mode)) {
        throw AccessViolationException("MMU::translate_address - Read not permitted on this page");
    }

    return entry.get_physical_address(virtual_address);
}

uint8_t MMU::read(uint32_t virtual_address) {
    uint32_t physical_address = translate_address(virtual_address, false);
    return physical_memory->read(physical_address);
}

void MMU::write(uint32_t virtual_address, uint8_t value) {
    uint32_t physical_address = translate_address(virtual_address, true);
    physical_memory->write(physical_address, value);
}

uint32_t MMU::read_word(uint32_t virtual_address) {
    // Read 4 bytes from memory and combine them into a 32-bit word
    uint32_t word = 0;
    word |= static_cast<uint32_t>(read(virtual_address));
    word |= static_cast<uint32_t>(read(virtual_address + 1)) << 8;
    word |= static_cast<uint32_t>(read(virtual_address + 2)) << 16;
    word |= static_cast<uint32_t>(read(virtual_address + 3)) << 24;
    return word;
}

void MMU::write_word(uint32_t virtual_address, uint32_t value) {
    // Write 4 bytes to memory from a 32-bit word
    write(virtual_address, static_cast<uint8_t>(value & 0xFF));
    write(virtual_address + 1, static_cast<uint8_t>((value >> 8) & 0xFF));
    write(virtual_address + 2, static_cast<uint8_t>((value >> 16) & 0xFF));
    write(virtual_address + 3, static_cast<uint8_t>((value >> 24) & 0xFF));
}

void MMU::set_privilege_mode(PrivilegeMode mode) {
    privilege_mode = mode;
}
