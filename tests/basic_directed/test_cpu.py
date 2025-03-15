import os
import tempfile
import unittest
import sys

from virtuv_bindings import CPU

class TestCPU(unittest.TestCase):
    def setUp(self):
        self.temp_files = []

    def tearDown(self):
        # Remove all temporary files created during tests.
        for f in self.temp_files:
            try:
                os.remove(f)
            except OSError:
                pass
        self.temp_files.clear()

    def _create_temp_program(self, program):
        """Helper to create a temporary file containing the given program instructions."""
        temp_file = tempfile.NamedTemporaryFile(delete=False)
        for instr in program:
            temp_file.write(instr.to_bytes(4, byteorder='little'))
        temp_file.flush()
        temp_file.close()
        self.temp_files.append(temp_file.name)
        return temp_file.name

    def test_cpu_execution(self):
        # Basic test with two addi instructions followed by a jump-to-self (end-of-program).
        program = [
            0x02A00093,  # addi x1, x0, 42
            0x3A08113,   # addi x2, x1, 58
            0x0000006F   # jal x0, 0 -> jump to self (end of program)
        ]
        prog_file = self._create_temp_program(program)

        cpu = CPU(1024 * 1024)
        result = cpu.load_program(prog_file)
        self.assertEqual(result, 0, "Program failed to load")

        try:
            cpu.run()
        except Exception as e:
            self.fail(f"Unexpected exception caught: {e}")

        # x0 is always 0, x1 should be 42, and x2 should be 42 + 58 = 100.
        self.assertEqual(cpu.get_register(0), 0, "x0 must be 0")
        self.assertEqual(cpu.get_register(1), 42, "x1 should be 42 after addi x1, x0, 42")
        self.assertEqual(cpu.get_register(2), 100, "x2 should be 100 after addi x2, x1, 58")

    def test_bubble_sort(self):
        # Test running a bubble sort program.
        unsorted_array = [5, 3, 4, 1, 2]
        bubble_sort_program = [
            0x00500293,  # addi t0, x0, 5
            0xFFF28293,  # addi t0, t0, -1
            0x00A2A023,  # beq t0, x0, offset_to_done 
            0x00030313,  # addi t1, x0, 0
            0x0023B393,  # slli t2, t1, 2   (shift left immediate 2)
            0x82000313,  # addi a0, x0, 0x2000   ; 0x2000 = 8192 decimal
            0x00C3C393,  # add t3, a0, t2   
            0x0003D413,  # lw t4, 0(t3)
            0x0043A213,  # addi t2, t2, 4
            0x00C3C593,  # add t5, a0, t2   
            0x0003F413,  # lw t6, 0(t5)
            0x00B3E393,  # bge t4, t6, swap   
            0x0000006F,  # j no_swap         ; jump to no_swap
            # swap:
            0x00C3E293,  # sw t6, 0(t3)    
            0x00C3E313,  # sw t4, 0(t5)    
            # no_swap:
            0x00130313,  # addi t1, t1, 1
            0x0002F393,  # blt t1, t0, inner_loop 
            0xFF9FF06F,  # j outer_loop  
            # done:
            0x0000006F   # j done          ; jump-to-self (end-of-program)
        ]
        prog_file = self._create_temp_program(bubble_sort_program)

        cpu = CPU(1024 * 1024)
        result = cpu.load_program(prog_file)
        self.assertEqual(result, 0, "Bubble sort program failed to load")

        try:
            cpu.run()
        except Exception as e:
            self.fail(f"Unexpected exception caught: {e}")

        base_array = 0x2000
        sorted_array = []
        for i in range(5):
            try:
                sorted_array.append(cpu.read_word_from_memory(base_array + i * 4))
            except Exception as e:
                self.fail(f"Error reading sorted array from memory at address {base_array + i * 4}: {e}")
        
        self.assertEqual(sorted_array, unsorted_array.sort(), "Bubble sort did not produce the expected sorted array")

    def test_invalid_instruction(self):
        """
        Test that the CPU throws an exception when encountering a non recognized instruction.
        """
        # Create a program with one valid instruction followed by an invalid opcode.
        program = [
            0x02A00093,  # Valid: addi x1, x0, 42
            0xFFFFFFFF   # Invalid instruction
        ]
        prog_file = self._create_temp_program(program)

        cpu = CPU(1024 * 1024)
        result = cpu.load_program(prog_file)
        self.assertEqual(result, 0, "Program failed to load")

        # Run the CPU and expect an exception due to the invalid instruction.
        with self.assertRaises(Exception):
            cpu.run()

if __name__ == "__main__":
    unittest.main()
