#pragma once

#include "gedouble/core.hpp"

#include <cstddef>
#include <cstdint>

namespace gedouble {

struct NoahhModMetadata {
    char id[96]{};
    char name[128]{};
    char version[32]{};
    char developer[128]{};
    char noahh_version[32]{};
    char android_game_version[24]{};
    char description[256]{};
};

struct NoahhPackageInfo {
    char filename[256]{};
    std::uint64_t size_bytes = 0;
};

/// Reads Noahh's mod.json metadata from a mod project or unpacked mod folder.
GEDOUBLE_EXPORT bool read_noahh_mod_json(
    const char* mod_json_path,
    NoahhModMetadata* output,
    char* error_buffer,
    std::size_t error_buffer_size
) noexcept;

/// Lists .noahh package files in a directory. Does not extract or execute them.
GEDOUBLE_EXPORT bool scan_noahh_packages(
    const char* mods_directory,
    NoahhPackageInfo* output,
    std::size_t output_capacity,
    std::size_t* package_count
) noexcept;

/// Documented shared-storage location used by Noahh Android.
GEDOUBLE_EXPORT const char* noahh_android_mods_relative_path() noexcept;

} // namespace gedouble
