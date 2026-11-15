#pragma once

#include "gedouble/core.hpp"

#include <cstddef>

namespace gedouble {

/// Native module callbacks. Callbacks must not throw exceptions.
struct ModuleDescriptor {
    const char* id;
    const char* version;
    bool (*on_load)() noexcept;
    void (*on_unload)() noexcept;
};

/// Loads and registers a module. The descriptor strings are copied internally.
GEDOUBLE_EXPORT bool register_module(const ModuleDescriptor& descriptor) noexcept;

/// Unloads a registered module by its unique identifier.
GEDOUBLE_EXPORT bool unregister_module(const char* id) noexcept;

/// Returns the number of currently registered modules.
GEDOUBLE_EXPORT std::size_t loaded_module_count() noexcept;

/// Checks whether a module identifier is currently registered.
GEDOUBLE_EXPORT bool is_module_loaded(const char* id) noexcept;

} // namespace gedouble
