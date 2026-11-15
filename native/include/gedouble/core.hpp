#pragma once

#include <cstdint>

#if defined(__GNUC__)
#define GEDOUBLE_EXPORT __attribute__((visibility("default")))
#else
#define GEDOUBLE_EXPORT
#endif

namespace gedouble {

enum class LogLevel : std::uint8_t {
    debug,
    info,
    warning,
    error
};

using LogCallback = void (*)(LogLevel level, const char* message) noexcept;

/// Returns the native core version.
GEDOUBLE_EXPORT const char* version() noexcept;

/// Initializes the Gedouble native runtime. Safe to call more than once.
GEDOUBLE_EXPORT bool initialize() noexcept;

/// Shuts down the Gedouble integration runtime.
GEDOUBLE_EXPORT void shutdown() noexcept;

/// Returns whether the runtime is currently initialized.
GEDOUBLE_EXPORT bool is_initialized() noexcept;

/// Installs the host application's log callback. Pass nullptr to clear it.
GEDOUBLE_EXPORT void set_log_callback(LogCallback callback) noexcept;

/// Writes a message to the configured host callback, if present.
GEDOUBLE_EXPORT void log(LogLevel level, const char* message) noexcept;

} // namespace gedouble
