# tests/test_mmu.py
import unittest
import sys
sys.path.insert(0, '/src/core/')
from virtuv_bindings import (
    PhysicalMemory,
    PageTableEntry,
    PageTable,
    MMU,
    PrivilegeMode,
    PageFaultException,
    AccessViolationException,
)

# Permission bits
VALID_BIT = 0x1
READ_BIT = 0x2
WRITE_BIT = 0x4
EXECUTE_BIT = 0x8
USER_ACCESSIBLE_BIT = 0x10


class TestMMU(unittest.TestCase):
    def setUp(self):
        # Create a PhysicalMemory of 1 MB
        self.physical_memory = PhysicalMemory(1024 * 1024)
        # Create a PageTable
        self.page_table = PageTable()
        # Set initial privilege mode to MACHINE
        self.mmu = MMU(self.physical_memory, self.page_table, PrivilegeMode.MACHINE)

        # Set up a valid page at virtual address 0x1000
        self.virtual_address = 0x1000
        self.page_number = self.virtual_address & 0xFFFFF000
        entry_value = (
                (self.page_number & 0xFFFFF000)
                | VALID_BIT
                | READ_BIT
                | WRITE_BIT
                | EXECUTE_BIT
                | USER_ACCESSIBLE_BIT
        )
        self.page_table.add_entry(self.page_number, PageTableEntry(entry_value))

    def test_read_write(self):
        # Write a byte to virtual memory
        self.mmu.write(self.virtual_address, 0xAB)
        # Read the byte back
        value = self.mmu.read(self.virtual_address)
        self.assertEqual(value, 0xAB)

    def test_read_write_word(self):
        # Write a word to virtual memory
        self.mmu.write_word(self.virtual_address, 0xDEADBEEF)
        # Read the word back
        value = self.mmu.read_word(self.virtual_address)
        self.assertEqual(value, 0xDEADBEEF)

    def test_access_violation_read(self):
        # Remove read permission but keep VALID_BIT
        entry_value = (
                (self.page_number & 0xFFFFF000)
                | VALID_BIT
                | WRITE_BIT
                | EXECUTE_BIT
                | USER_ACCESSIBLE_BIT  # No READ_BIT
        )
        self.page_table.add_entry(self.page_number, PageTableEntry(entry_value))
        with self.assertRaises(AccessViolationException):
            self.mmu.read(self.virtual_address)

    def test_access_violation_write(self):
        # Remove write permission but keep VALID_BIT
        entry_value = (
                (self.page_number & 0xFFFFF000)
                | VALID_BIT
                | READ_BIT
                | EXECUTE_BIT
                | USER_ACCESSIBLE_BIT  # No WRITE_BIT
        )
        self.page_table.add_entry(self.page_number, PageTableEntry(entry_value))
        with self.assertRaises(AccessViolationException):
            self.mmu.write(self.virtual_address, 0xAB)

    def test_page_fault(self):
        # Access a virtual address without a valid page table entry
        invalid_address = 0x2000  # Different from self.virtual_address
        with self.assertRaises(PageFaultException):
            self.mmu.read(invalid_address)

    def test_privilege_mode(self):
        # Remove user access but keep VALID_BIT and READ_BIT
        entry_value = (
                (self.page_number & 0xFFFFF000)
                | VALID_BIT
                | READ_BIT
                | WRITE_BIT
                | EXECUTE_BIT  # No USER_ACCESSIBLE_BIT
        )
        self.page_table.add_entry(self.page_number, PageTableEntry(entry_value))

        # Read should fail in USER mode due to lack of user access
        self.mmu.set_privilege_mode(PrivilegeMode.USER)
        with self.assertRaises(AccessViolationException):
            self.mmu.read(self.virtual_address)

        # Read should succeed in MACHINE mode
        self.mmu.set_privilege_mode(PrivilegeMode.MACHINE)
        self.mmu.read(self.virtual_address)

    def test_translate_address(self):
        # Test that virtual to physical address translation works correctly
        physical_address = self.mmu.translate_address(
            self.virtual_address, is_write=False
        )
        expected_physical_address = (
                (self.page_number & 0xFFFFF000)
                | (self.virtual_address & 0xFFF)
        )
        self.assertEqual(physical_address, expected_physical_address)


if __name__ == "__main__":
    unittest.main()
