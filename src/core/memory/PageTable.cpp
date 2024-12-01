#include "PageTable.hpp"
#include <stdexcept>

#include "MMU.hpp"

void PageTable::add_entry(uint32_t virtual_address, PageTableEntry entry) {
    entries[virtual_address] = entry;
}

PageTableEntry PageTable::get_entry(uint32_t virtual_address) {
    auto it = entries.find(virtual_address & 0xFFFFF000); //Page size == 4KB
    if (it != entries.end()) {
        return it->second;
    } else {
        throw PageFaultException("PageTable::get_entry - No entry for virtual address");
    }
}