#include "gedouble/core.hpp"

namespace gedouble {
namespace {
constexpr char kVersion[] = "0.1.0-dev";
}

const char* version() noexcept {
    return kVersion;
}

bool is_initialized() noexcept {
    // The runtime is only a scaffold at this stage.
    return true;
}

} // namespace gedouble
