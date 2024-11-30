#pragma once
#include <cstdint>
#include <initializer_list>
#include <utility>
#include <variant>
#include "utils/bitutils.hpp"

//Values are the opcodes for the matching instructions
enum class InstructionFormat {
    R_TYPE          = 0x33, 
    I_TYPE          = 0x13,
    S_TYPE          = 0x23, 
    B_TYPE          = 0x63,
    U_TYPE          = 0x37,
    J_TYPE          = 0x6F,
    INIVALID_TYPE   = 0xFF
};

// Combines bit ranges from the instruction into a single immediate value
template <typename Instruction>
constexpr int32_t get_combined_immediate(const Instruction& instr,
                                         std::initializer_list<std::pair<int, int>> bit_ranges) {
    int32_t immediate = 0;
    // Iterate over each source range (start to end)
    for (const auto& [start, end] : bit_ranges) {
        for (int bit = start; bit >= end; --bit) { // Include all bits in the range
            int32_t dst_bit = bit - end;          // Align destination bits to zero
            immediate |= ((instr.raw >> bit) & 1) << dst_bit;
        }
    }
    return immediate;
}

// Base class for decoded instructions
class DecodedInstructionBase {
public:
    static constexpr InstructionFormat format = InstructionFormat::INIVALID_TYPE;
    union {
        uint32_t raw; // Full 32-bit raw instruction
        struct {
            uint32_t opcode : 7;  // Bits [6:0]
            uint32_t rest : 25;      // Bits [31:7]
        };
    };

    DecodedInstructionBase() = default;

    explicit DecodedInstructionBase(uint32_t raw_instruction) : raw(raw_instruction) {}

    virtual ~DecodedInstructionBase() = default;
    virtual uint32_t get_opcode() const {
        return opcode;
    }
};

// Base template for DecodedInstruction
template <InstructionFormat Format>
class DecodedInstruction;

//Specialization fro invalid type (just for initilization and throwing exceptions when something is not working properly)
template <>
class DecodedInstruction<InstructionFormat::INIVALID_TYPE> : public DecodedInstructionBase {
public:
    static constexpr InstructionFormat format = InstructionFormat::INIVALID_TYPE;

    explicit DecodedInstruction(uint32_t raw_instruction) : DecodedInstructionBase(raw_instruction) {}

    uint32_t get_opcode() const override {
        throw std::runtime_error("Attempting to access an invalid instruction format");
    }
};

// Specialization for R-type instructions
template <>
class DecodedInstruction<InstructionFormat::R_TYPE> : DecodedInstructionBase{
public:
    static constexpr InstructionFormat format = InstructionFormat::R_TYPE;
    union {
        uint32_t raw; // Full 32-bit raw instruction
        struct {
            uint32_t opcode : 7;  // Bits [6:0]
            uint32_t rd : 5;      // Bits [11:7]
            uint32_t funct3 : 3;  // Bits [14:12]
            uint32_t rs1 : 5;     // Bits [19:15]
            uint32_t rs2 : 5;     // Bits [24:20]
            uint32_t funct7 : 7;  // Bits [31:25]
        };
    };

    explicit DecodedInstruction(uint32_t instruction) : raw(instruction) {}

    uint32_t get_opcode() const override {
        return opcode;
    }
};

// Specialization for I-type instructions
template <>
class DecodedInstruction<InstructionFormat::I_TYPE> : DecodedInstructionBase{
public:
    static constexpr InstructionFormat format = InstructionFormat::I_TYPE;
    union {
        uint32_t raw; // Full 32-bit raw instruction
        struct {
            uint32_t opcode : 7;  // Bits [6:0]
            uint32_t rd : 5;      // Bits [11:7]
            uint32_t funct3 : 3;  // Bits [14:12]
            uint32_t rs1 : 5;     // Bits [19:15]
            int32_t imm : 12;     // Bits [31:20], sign-extended
        }; 
    };

    explicit DecodedInstruction(uint32_t instruction) : raw(instruction) {}

    int32_t get_immediate() const {
        // I-Type immediate is a contiguous range [31:20]
        return bitutils::sign_extend(get_combined_immediate(*this, {
            {31, 20} // Immediate field
        }), 12);
    }

    uint32_t get_opcode() const override {
        return opcode;
    }
};

// Specialization for S-type instructions
template <>
class DecodedInstruction<InstructionFormat::S_TYPE> : DecodedInstructionBase {
public:
    static constexpr InstructionFormat format = InstructionFormat::S_TYPE;
    union {
        uint32_t raw; // Full 32-bit raw instruction
        struct {
            uint32_t opcode : 7;  // Bits [6:0]
            uint32_t imm4_0 : 5;  // Bits [11:7] (immediate low bits)
            uint32_t funct3 : 3;  // Bits [14:12]
            uint32_t rs1 : 5;     // Bits [19:15]
            uint32_t rs2 : 5;     // Bits [24:20]
            uint32_t imm11_5 : 7; // Bits [31:25] (immediate high bits)
        };
    };

    explicit DecodedInstruction(uint32_t instruction) : raw(instruction) {}

    int32_t get_immediate() const {
        // S-Type immediate spans two ranges: [31:25] and [11:7]
        return bitutils::sign_extend(get_combined_immediate(*this, {
            {31, 25}, // Upper immediate [11:5]
            {11, 7}   // Lower immediate [4:0]
        }), 12);
    }

    uint32_t get_opcode() const override {
        return opcode;
    }
};

// Specialization for B-type instructions
template <>
class DecodedInstruction<InstructionFormat::B_TYPE> : DecodedInstructionBase {
public:
    static constexpr InstructionFormat format = InstructionFormat::B_TYPE;
    union {
        uint32_t raw; // Full 32-bit raw instruction
        struct {
            uint32_t opcode : 7;  // Bits [6:0]
            uint32_t imm11 : 1;   // Bit [7] -> Immediate bit 11
            uint32_t imm4_1 : 4;  // Bits [11:8] -> Immediate bits [4:1]
            uint32_t funct3 : 3;  // Bits [14:12]
            uint32_t rs1 : 5;     // Bits [19:15]
            uint32_t rs2 : 5;     // Bits [24:20]
            uint32_t imm10_5 : 6; // Bits [30:25] -> Immediate bits [10:5]
            uint32_t imm12 : 1;   // Bit [31] -> Immediate bit 12
        };
    };

    explicit DecodedInstruction(uint32_t instruction) : raw(instruction) {}

    int32_t get_immediate() const {
        // B-Type immediate spans four ranges: [31], [30:25], [11:8], and [7]
        return bitutils::sign_extend(get_combined_immediate(*this, {
            {31, 31}, // imm[12]
            {30, 25}, // imm[10:5]
            {11, 8},  // imm[4:1]
            {7, 7}    // imm[11]
        }), 13);
    }

    uint32_t get_opcode() const override {
        return opcode;
    }
};

// Specialization for U-type instructions
template <>
class DecodedInstruction<InstructionFormat::U_TYPE> : DecodedInstructionBase{
public:
    static constexpr InstructionFormat format = InstructionFormat::U_TYPE;
    union {
        uint32_t raw; // Full 32-bit raw instruction
        struct {
            uint32_t opcode : 7;  // Bits [6:0]
            uint32_t rd : 5;      // Bits [11:7]
            uint32_t imm : 20;    // Bits [31:12] (upper 20 bits of immediate)
        }; 
    };

    explicit DecodedInstruction(uint32_t instruction) : raw(instruction) {}

    int32_t get_immediate() const {
        // U-Type immediate is a contiguous range [31:12]
        return get_combined_immediate(*this, {
            {31, 12} // Immediate field
        }) << 12; // Shift left by 12 bits
    }

    uint32_t get_opcode() const override {
        return opcode;
    }
};

// Specialization for J-type instructions
template <>
class DecodedInstruction<InstructionFormat::J_TYPE> : DecodedInstructionBase{
public:
    static constexpr InstructionFormat format = InstructionFormat::J_TYPE;
    union {
        uint32_t raw; // Full 32-bit raw instruction
        struct {
            uint32_t opcode : 7;   // Bits [6:0]
            uint32_t rd : 5;       // Bits [11:7]
            uint32_t imm19_12 : 8; // Bits [19:12]
            uint32_t imm11 : 1;    // Bit [20]
            uint32_t imm10_1 : 10; // Bits [30:21]
            uint32_t imm20 : 1;    // Bit [31]
        }; 
    };

    explicit DecodedInstruction(uint32_t instruction) : raw(instruction) {}

    int32_t get_immediate() const {
        // J-Type immediate spans four ranges: [31], [30:21], [20], and [19:12]
        return bitutils::sign_extend(get_combined_immediate(*this, {
            {31, 31}, // imm[20]
            {30, 21}, // imm[10:1]
            {20, 20}, // imm[11]
            {19, 12}  // imm[19:12]
        }), 21);
    }

    uint32_t get_opcode() const override {
        return opcode;
    }
};

// Define the variant type to hold all possible instruction formats
using DecodedInstructionVariant = std::variant<
    DecodedInstruction<InstructionFormat::INIVALID_TYPE>,
    DecodedInstruction<InstructionFormat::R_TYPE>,
    DecodedInstruction<InstructionFormat::I_TYPE>,
    DecodedInstruction<InstructionFormat::S_TYPE>,
    DecodedInstruction<InstructionFormat::B_TYPE>,
    DecodedInstruction<InstructionFormat::U_TYPE>,
    DecodedInstruction<InstructionFormat::J_TYPE>
>;