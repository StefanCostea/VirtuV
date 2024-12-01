#pragma once
#include <unordered_map>
#include "PageTableEntry.hpp"
/**
 * @brief Represents the page table used for virtual to physical address translation.
 *
 * Manages the mappings of virtual addresses to physical addresses via page table entries.
 */
class PageTable {
private:
    std::unordered_map<uint32_t, PageTableEntry> entries;

public:
    /**
     * @brief Adds a page table entry for a given virtual address.
     * @param virtual_address The virtual address of the page.
     * @param entry The page table entry associated with the virtual address.
     */
    void add_entry(uint32_t virtual_address, PageTableEntry entry);

    /**
     * @brief Retrieves the page table entry for a given virtual address.
     * @param virtual_address The virtual address to look up.
     * @return The page table entry associated with the virtual address.
     * @throws std::out_of_range if no entry exists for the given address.
     */
    PageTableEntry get_entry(uint32_t virtual_address);
};
