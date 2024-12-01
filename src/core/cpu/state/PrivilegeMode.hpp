#pragma once
/**
 * @brief Enum representing the CPU privilege modes.
 *
 * RISC-V defines several privilege levels: User (U), Supervisor (S), and Machine (M).
 */
enum class PrivilegeMode {
    USER = 0,
    SUPERVISOR = 1,
    MACHINE = 3
};