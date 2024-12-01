#pragma once
#include <stdexcept>

#include "PhysicalMemory.hpp"
#include "PageTable.hpp"
#include "core/cpu/state/PrivilegeMode.hpp"

//Todo: define this in other place eg memoryexceptions
class PageFaultException : public std::runtime_error {
public:
 explicit PageFaultException(const std::string& msg) : std::runtime_error(msg) {}
};

class AccessViolationException : public std::runtime_error {
public:
 explicit AccessViolationException(const std::string& msg) : std::runtime_error(msg) {}
};

/**
 * @brief Memory Management Unit responsible for address translation and access control.
 *
 * The MMU translates virtual addresses to physical addresses using the page table
 * and enforces access permissions based on the CPU's privilege mode.
 */
class MMU {
private:
    PhysicalMemory* physical_memory;
    PageTable* page_table;
    PrivilegeMode privilege_mode;

public:
    /**
     * @brief Constructs an MMU with the given physical memory, page table, and privilege mode.
     * @param phys_mem Pointer to the physical memory.
     * @param pt Pointer to the page table.
     * @param mode The current privilege mode.
     */
    MMU(PhysicalMemory* phys_mem, PageTable* pt, PrivilegeMode mode);

    /**
     * @brief Translates a virtual address to a physical address.
     * @param virtual_address The virtual address to translate.
     * @param is_write Indicates if the access is a write operation.
     * @return The corresponding physical address.
     * @throws PageFaultException if the page is not valid.
     * @throws AccessViolationException if access is not permitted.
     */
    uint32_t translate_address(uint32_t virtual_address, bool is_write);

    /**
     * @brief Reads a byte from a virtual memory address.
     * @param virtual_address The virtual address to read from.
     * @return The byte value at the specified address.
     */
    uint8_t read(uint32_t virtual_address);

    /**
     * @brief Writes a byte to a virtual memory address.
     * @param virtual_address The virtual address to write to.
     * @param value The byte value to write.
     */
    void write(uint32_t virtual_address, uint8_t value);

    /**
     * @brief Reads a 32-bit word from a virtual memory address.
     * @param virtual_address The virtual address to read from.
     * @return The 32-bit word at the specified address.
     */
    uint32_t read_word(uint32_t virtual_address);

    /**
     * @brief Writes a 32-bit word to a virtual memory address.
     * @param virtual_address The virtual address to write to.
     * @param value The 32-bit word to write.
     */
    void write_word(uint32_t virtual_address, uint32_t value);

    /**
     * @brief Sets the current privilege mode of the MMU.
     * @param mode The new privilege mode.
     */
    void set_privilege_mode(PrivilegeMode mode);
};