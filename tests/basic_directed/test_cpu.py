import os
import tempfile
import unittest
import sys
sys.path.insert(0, '/src/core/')
from virtuv_bindings import CPU  

class TestCPU(unittest.TestCase):
    def setUp(self):
        # temp file to hold program
        self.temp_file = tempfile.NamedTemporaryFile(delete=False)
        
        program = [
            0x02A00093,  # addi x1, x0, 42
            0x3A08113,   # addi x2, x1, 58
            0xFFFFFFFF   # invalid instruction (to stop execution)
        ]
        
        for instr in program:
            self.temp_file.write(instr.to_bytes(4, byteorder='little'))
        self.temp_file.flush()
        self.temp_file.close()

    def tearDown(self):
        os.remove(self.temp_file.name)

    def test_cpu_execution(self):
        # Create a CPU instance with 1MB of physical memory.
        cpu = CPU(1024 * 1024)
        
        # Load the program from our temporary file.
        result = cpu.load_program(self.temp_file.name)
        self.assertEqual(result, 0, "Program failed to load")

        # Run cpu
        try:
            cpu.run()
        except Exception as e:
            print(e)
            pass

        # x0 is always 0, x1 should be 42, and x2 should be 42 + 58 = 100.
        self.assertEqual(cpu.get_register(0), 0, "x0 must be 0")
        self.assertEqual(cpu.get_register(1), 42, "x1 should be 42 after addi x1, x0, 42")
        self.assertEqual(cpu.get_register(2), 100, "x2 should be 100 after addi x2, x1, 58")

if __name__ == "__main__":
    unittest.main()
