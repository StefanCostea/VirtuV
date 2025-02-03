#!/usr/bin/env python
import unittest
import sys
sys.path.insert(0, '/src/core/')
from virtuv_bindings import (
    PhysicalMemory,
    PageTable,
    PageTableEntry,
    MMU,
    RegisterBank,
    FetchStage,
    DecodeStage,
    ExecuteStage,
    MemoryAccessStage,
    WriteBackStage,
    PrivilegeMode,
    ExecutionResult,
    MemoryAccessResult,
    DecodedInstructionInvalid,
    DecodedInstructionRType,
    DecodedInstructionIType,
    DecodedInstructionSType,
    DecodedInstructionUType,
    InstructionFormat
)

VALID_BIT = 0x1
READ_BIT = 0x2
WRITE_BIT = 0x4
EXECUTE_BIT = 0x8
USER_ACCESSIBLE_BIT = 0x10


# -------------------------------------------------------
# Test the FetchStage
# -------------------------------------------------------
class TestFetchStage(unittest.TestCase):
    def setUp(self):
        self.reg_bank = RegisterBank()
        self.phys_mem = PhysicalMemory(1024 * 1024) # 1MB memory
        self.page_table = PageTable()
        page_address = 0x0000
        entry_value = (page_address & 0xFFFFF000) | VALID_BIT | READ_BIT | WRITE_BIT | EXECUTE_BIT | USER_ACCESSIBLE_BIT
        self.page_table.add_entry(page_address, PageTableEntry(entry_value))
        self.mmu = MMU(self.phys_mem, self.page_table, PrivilegeMode.MACHINE)
    
    def test_fetch_instruction(self):
        # Write a known 32-bit instruction at address 0
        test_instruction = 0x12345678
        self.mmu.write_word(0, test_instruction)
        self.reg_bank.set_pc(0)
        
        # Create and process the fetch stage
        fetch_stage = FetchStage(self.mmu, self.reg_bank)
        fetch_stage.process()
        fetched = fetch_stage.get_fetched_instruction()
        self.assertEqual(fetched, test_instruction)

# -------------------------------------------------------
# Test the DecodeStage
# -------------------------------------------------------
class TestDecodeStage(unittest.TestCase):
    def setUp(self):
        self.reg_bank = RegisterBank()
        self.decode_stage = DecodeStage(self.reg_bank)
    
    def test_decode_r_type(self):
        # create R-type instruction
        # ADD instruction with:
        #   funct7 = 0, rs2 = 2, rs1 = 1, funct3 = 0, rd = 3, opcode = 0x33
        instruction = (0x00 << 25) | (2 << 20) | (1 << 15) | (0 << 12) | (3 << 7) | 0x33
        self.decode_stage.set_fetched_instruction(instruction)
        self.decode_stage.process()
        decoded = self.decode_stage.get_decoded_instruction()
        
        # Check that the decoded instruction fields match
        self.assertEqual(decoded.rs1, 1)
        self.assertEqual(decoded.rs2, 2)
        self.assertEqual(decoded.rd, 3)
        self.assertEqual(decoded.funct3, 0)
        self.assertEqual(decoded.funct7, 0)

# -------------------------------------------------------
# Test the ExecuteStage 
# -------------------------------------------------------
class TestExecuteStage(unittest.TestCase):
    def setUp(self):
        self.reg_bank = RegisterBank()
        # For an ADDI instruction, set register x1 to 10
        self.reg_bank.write(1, 10)
        self.execute_stage = ExecuteStage(self.reg_bank)
    
    def test_execute_addi(self):
        # Create dummy I-type ADDI instruction: addi x2, x1, 5.
        #   opcode 0x13 for ADDI, funct3 = 0, rs1 = 1, rd = 2, immediate = 5
        addi_inst = DecodedInstructionIType(0)
        addi_inst.opcode = 0x13
        addi_inst.rs1 = 1
        addi_inst.rd = 2
        addi_inst.funct3 = 0
        addi_inst.imm = 5 
        
        self.execute_stage.set_decoded_instruction(addi_inst)
        self.execute_stage.process()
        exec_result = self.execute_stage.get_result()
        self.assertEqual(exec_result.alu_result, 15)  # 10 + 5 = 15

# -------------------------------------------------------
# Test the MemoryAccessStage individually
# -------------------------------------------------------
class TestMemoryAccessStage(unittest.TestCase):
    def setUp(self):
        self.reg_bank = RegisterBank()
        self.phys_mem = PhysicalMemory(1024 * 1024)
        self.page_table = PageTable()
        # Map a page covering address 0x1000
        virtual_address = 0x1000
        page_address = virtual_address & 0xFFFFF000
        entry_value = (page_address & 0xFFFFF000) | VALID_BIT | READ_BIT | WRITE_BIT | EXECUTE_BIT | USER_ACCESSIBLE_BIT
        self.page_table.add_entry(page_address, PageTableEntry(entry_value))
        self.mmu = MMU(self.phys_mem, self.page_table, PrivilegeMode.MACHINE)
    
    def test_memory_access_load(self):
        # Test a load operation
        virtual_address = 0x1000
        test_value = 0xCAFEBABE
        self.mmu.write_word(virtual_address, test_value)
        
        # Create an execution result with the effective address
        exec_result = ExecutionResult()
        exec_result.alu_result = virtual_address
        
        # Create a dummy I-type load instruction (using opcode 0x03 for loads)
        load_inst = DecodedInstructionIType(0)
        load_inst.opcode = 0x03
        load_inst.rd = 5  # destination register 
        
        mem_stage = MemoryAccessStage(self.mmu, self.reg_bank)
        mem_stage.set_execution_result(exec_result)
        mem_stage.set_decoded_instruction(load_inst)
        mem_stage.process()
        mem_result = mem_stage.get_result()
        
        self.assertIsNotNone(mem_result.load_data)
        self.assertEqual(mem_result.load_data, test_value)
    
    def test_memory_access_store(self):
        # Test a store operation
        virtual_address = 0x2000
        page_address = virtual_address & 0xFFFFF000
        entry_value = (page_address & 0xFFFFF000) | VALID_BIT | READ_BIT | WRITE_BIT | EXECUTE_BIT | USER_ACCESSIBLE_BIT
        self.page_table.add_entry(page_address, PageTableEntry(entry_value))
        
        exec_result = ExecutionResult()
        exec_result.alu_result = virtual_address
        
        # Create a dummy S-type store instruction
        store_inst = DecodedInstructionSType(0)
        store_inst.opcode = 0x23  # store opcode
        store_inst.rs2 = 4  
        
        # Set register x4
        self.reg_bank.write(4, 0xDEADBEEF)
        
        mem_stage = MemoryAccessStage(self.mmu, self.reg_bank)
        mem_stage.set_execution_result(exec_result)
        mem_stage.set_decoded_instruction(store_inst)
        mem_stage.process()
        stored_value = self.phys_mem.read_word(virtual_address)
        self.assertEqual(stored_value, 0xDEADBEEF)

# -------------------------------------------------------
# Test the WriteBackStage individually
# -------------------------------------------------------
class TestWriteBackStage(unittest.TestCase):
    def setUp(self):
        self.reg_bank = RegisterBank()
    
    def test_write_back_load(self):
        # Simulate a load that writes back a value into a register
        exec_result = ExecutionResult()  # not used for loads in this simple test
        mem_result = MemoryAccessResult()
        mem_result.load_data = 0xABCDEF12
        
        # Create a dummy I-type load instruction with destination register x7
        load_inst = DecodedInstructionIType(0)
        load_inst.opcode = 0x03  # load opcode
        load_inst.rd = 7
        
        wb_stage = WriteBackStage(self.reg_bank)
        wb_stage.set_execution_result(exec_result)
        wb_stage.set_memory_access_result(mem_result)
        wb_stage.set_decoded_instruction(load_inst)
        wb_stage.process()
        
        # Verify that register x7 now contains the loaded value
        self.assertEqual(self.reg_bank.read(7), 0xABCDEF12)

# -------------------------------------------------------
# Run the tests.
# -------------------------------------------------------
if __name__ == '__main__':
    unittest.main()
