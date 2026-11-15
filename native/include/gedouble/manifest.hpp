#pragma once

#include "gedouble/core.hpp"

#include <cstddef>

namespace gedouble {

struct ModManifest {
    char id[96]{};
    char name[128]{};
    char version[32]{};
    char entry[128]{};
    char description[256]{};
};

/// Reads and validates one manifest.json file.
GEDOUBLE_EXPORT bool read_mod_manifest(
    const char* manifest_path,
    ModManifest* output,
    char* error_buffer,
    std::size_t error_buffer_size
) noexcept;

/// Scans immediate subdirectories of root_path for manifest.json files.
/// Returns false on invalid arguments or filesystem errors. The number of
/// accepted manifests is always written to discovered_count when non-null.
GEDOUBLE_EXPORT bool discover_mods(
    const char* root_path,
    ModManifest* output,
    std::size_t output_capacity,
    std::size_t* discovered_count
) noexcept;

} // namespace gedouble
