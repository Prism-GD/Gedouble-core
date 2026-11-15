#pragma once

#if defined(__GNUC__)
#define GEDOUBLE_EXPORT __attribute__((visibility("default")))
#else
#define GEDOUBLE_EXPORT
#endif

namespace gedouble {

/// Returns the native core version as a stable, human-readable string.
GEDOUBLE_EXPORT const char* version() noexcept;

/// Returns true when the native core library has been loaded.
GEDOUBLE_EXPORT bool is_initialized() noexcept;

} // namespace gedouble
