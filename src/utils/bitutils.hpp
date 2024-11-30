#pragma once

namespace bitutils{

/**
 * @brief Sign-extends an integer to its full type width.
 *
 * Converts a smaller signed integer (e.g., 13-bit) to a larger signed
 * integer by replicating the sign bit.
 */
template <typename T>
constexpr T sign_extend(T value, int bit_width) {
    const T sign_bit = 1 << (bit_width - 1);  // Compute the sign bit position
    return (value ^ sign_bit) - sign_bit;    // Flip and subtract sign bit for extension
}

};