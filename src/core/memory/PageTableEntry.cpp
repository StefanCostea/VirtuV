#include "PageTableEntry.hpp"

PageTableEntry::PageTableEntry(uint32_t value) : entry_value(value) {}

bool PageTableEntry::is_valid() const {
    return entry_value & VALID_BIT;
}

bool PageTableEntry::is_readable(PrivilegeMode mode) const {
    if (!is_valid()) return false;
    if (!(entry_value & READ_BIT)) return false;
    if (mode == PrivilegeMode::USER && !(entry_value & USER_ACCESSIBLE_BIT)) return false;
    return true;
}

bool PageTableEntry::is_writable(PrivilegeMode mode) const {
    if (!is_valid()) return false;
    if (!(entry_value & WRITE_BIT)) return false;
    if (mode == PrivilegeMode::USER && !(entry_value & USER_ACCESSIBLE_BIT)) return false;
    return true;
}

bool PageTableEntry::is_executable(PrivilegeMode mode) const {
    if (!is_valid()) return false;
    if (!(entry_value & EXECUTE_BIT)) return false;
    if (mode == PrivilegeMode::USER && !(entry_value & USER_ACCESSIBLE_BIT)) return false;
    return true;
}

uint32_t PageTableEntry::get_physical_address(uint32_t virtual_address) const {
    // Assuming direct mapping (no offset calculation)
    // In reality, would extract the physical page number and combine it with the offset
    return (entry_value & 0xFFFFF000) | (virtual_address & 0xFFF);
}