#pragma once
#include "core/cpu/state/PrivilegeMode.hpp"
#include <cstdint>
/**
 * @brief Represents an entry in the page table.
 *
 * Contains information about the mapping between virtual and physical addresses
 * and the permissions associated with the page.
 */
class PageTableEntry {
private:
    uint32_t entry_value; /**< The raw value of the page table entry */
public:
    static constexpr uint32_t VALID_BIT = 0x1; //Indicates the validity of the entry 01
    static constexpr uint32_t READ_BIT = 0x2; //Allow read operations 10
    static constexpr uint32_t WRITE_BIT = 0x4; // Allow write operations 100
    static constexpr uint32_t EXECUTE_BIT = 0x8; // Allow execution operations 1000
    static constexpr uint32_t USER_ACCESSIBLE_BIT = 0x10; //Indicates if the page is accessible in USER mode 1010
public:
    /**
     * @brief Default constructor for an invalid entry.
     */
    PageTableEntry() : entry_value(0) {}

    /**
     * @brief Constructs a PageTableEntry with a given raw value.
     * @param value The raw value representing the page table entry.
     */
    explicit PageTableEntry(uint32_t value);
    /**
     * @brief Checks if the page entry is valid.
     * @return True if the page entry is valid; otherwise, false.
     */
    bool is_valid() const;
    /**
     * @brief Checks if the page is readable in the given privilege mode.
     * @param mode The current privilege mode of the CPU.
     * @return True if readable; otherwise, false.
     */
    bool is_readable(PrivilegeMode mode) const;
    /**
     * @brief Checks if the page is writable in the given privilege mode.
     * @param mode The current privilege mode of the CPU.
     * @return True if writable; otherwise, false.
     */
    bool is_writable(PrivilegeMode mode) const;
    /**
     * @brief Checks if the page is executable in the given privilege mode.
     * @param mode The current privilege mode of the CPU.
     * @return True if executable; otherwise, false.
     */
    bool is_executable(PrivilegeMode mode) const;
    /**
      * @brief Gets the physical address corresponding to the virtual address.
      * @param virtual_address The virtual address to translate.
      * @return The corresponding physical address.
      */
    uint32_t get_physical_address(uint32_t virtual_address) const;
};