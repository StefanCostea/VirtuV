#pragma once

#include <sstream>
#include <iostream>
#include <string>
#include <utility>

// You can disable debug output by defining NO_DEBUG and
// enable trace info by defining ENABLE_TRACE during compilation.

namespace plt {

// ANSI escape sequences for colorized output
constexpr const char* RESET   = "\033[0m";
constexpr const char* RED     = "\033[1;31m";
constexpr const char* GREEN   = "\033[1;32m";
constexpr const char* YELLOW  = "\033[1;33m";
constexpr const char* CYAN    = "\033[1;36m";

#ifndef NO_DEBUG
inline bool debug_enabled = true;
#else
inline bool debug_enabled = false;
#endif

inline void enable_debug() noexcept { debug_enabled = true; }
inline void disable_debug() noexcept { debug_enabled = false; }

inline void debug(const std::string &msg) {
    if (debug_enabled)
        std::cerr << CYAN << "[DEBUG] " << RESET << msg << '\n';
}

inline void info(const std::string &msg) {
    std::cout << GREEN << "[INFO] " << RESET << msg << '\n';
}

inline void warn(const std::string &msg) {
    std::cerr << YELLOW << "[WARN] " << RESET << msg << '\n';
}

inline void error(const std::string &msg) {
    std::cerr << RED << "[ERROR] " << RESET << msg << '\n';
}

// A simple concatenation helper that uses an ostringstream.
template<typename... Args>
[[nodiscard]] inline std::string concat(Args&&... args) {
    std::ostringstream oss;
    ((oss << " " << std::forward<Args>(args)), ...);
    return oss.str();
}

// Formatted logging functions: they simply concatenate the format string and the arguments.
template<typename... Args>
inline void debugf(const std::string &fmt, Args&&... args) {
    if (debug_enabled)
        std::cerr << CYAN << "[DEBUG] " << RESET << concat(fmt, std::forward<Args>(args)...) << '\n';
}

template<typename... Args>
inline void infof(const std::string &fmt, Args&&... args) {
    std::cout << GREEN << "[INFO] " << RESET << concat(fmt, std::forward<Args>(args)...) << '\n';
}

template<typename... Args>
inline void warnf(const std::string &fmt, Args&&... args) {
    std::cerr << YELLOW << "[WARN] " << RESET << concat(fmt, std::forward<Args>(args)...) << '\n';
}

template<typename... Args>
inline void errorf(const std::string &fmt, Args&&... args) {
    std::cerr << RED << "[ERROR] " << RESET << concat(fmt, std::forward<Args>(args)...) << '\n';
}

} // namespace plt

// --- Macros to automatically capture file, line, and function for logging ---
//
// If ENABLE_TRACE is defined (e.g., by passing -DENABLE_TRACE to the compiler),
// these macros will prepend the call-site information to the message.
// Otherwise, they simply call the corresponding plt functions.

#ifdef ENABLE_TRACE
  #define PLT_INFO(msg)  plt::info(plt::concat("[", __FILE__, ":", __LINE__, " ", __func__, "] ", msg))
  #define PLT_WARN(msg)  plt::warn(plt::concat("[", __FILE__, ":", __LINE__, " ", __func__, "] ", msg))
  #define PLT_ERROR(msg) plt::error(plt::concat("[", __FILE__, ":", __LINE__, " ", __func__, "] ", msg))
  #define PLT_DEBUG(msg) plt::debug(plt::concat("[", __FILE__, ":", __LINE__, " ", __func__, "] ", msg))
  #define PLT_DEBUG_VARS(...) plt::debug(plt::concat("[", __FILE__, ":", __LINE__, " ", __func__, "]", __VA_ARGS__))
  #define PLT_INFOF(fmt, ...)  plt::infof(plt::concat("[", __FILE__, ":", std::to_string(__LINE__), " ", __func__, "] ", fmt), __VA_ARGS__)
  #define PLT_WARNF(fmt, ...)  plt::warnf(plt::concat("[", __FILE__, ":", std::to_string(__LINE__), " ", __func__, "] ", fmt), __VA_ARGS__)
  #define PLT_ERRORF(fmt, ...) plt::errorf(plt::concat("[", __FILE__, ":", std::to_string(__LINE__), " ", __func__, "] ", fmt), __VA_ARGS__)
  #define PLT_DEBUGF(fmt, ...) plt::debugf(plt::concat("[", __FILE__, ":", std::to_string(__LINE__), " ", __func__, "] ", fmt), __VA_ARGS__)
#else
  #define PLT_INFO(msg)  plt::info(msg)
  #define PLT_WARN(msg)  plt::warn(msg)
  #define PLT_ERROR(msg) plt::error(msg)
  #define PLT_DEBUG(msg) plt::debug(msg)
  #define PLT_DEBUG_VARS(...) plt::debug(plt::concat(__VA_ARGS__))
  #define PLT_INFOF(fmt, ...)  plt::infof(fmt, __VA_ARGS__)
  #define PLT_WARNF(fmt, ...)  plt::warnf(fmt, __VA_ARGS__)
  #define PLT_ERRORF(fmt, ...) plt::errorf(fmt, __VA_ARGS__)
  #define PLT_DEBUGF(fmt, ...) plt::debugf(fmt, __VA_ARGS__)
#endif
