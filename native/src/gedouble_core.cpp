#include "gedouble/core.hpp"

#include <atomic>
#include <mutex>

namespace gedouble {
namespace {
constexpr char kVersion[] = "0.2.0-dev";
std::atomic_bool g_initialized{false};
std::mutex g_log_mutex;
LogCallback g_log_callback = nullptr;
}

const char* version() noexcept {
    return kVersion;
}

bool initialize() noexcept {
    bool expected = false;
    if (g_initialized.compare_exchange_strong(expected, true)) {
        log(LogLevel::info, "Gedouble-Core runtime initialized");
    }
    return true;
}

void shutdown() noexcept {
    if (!g_initialized.exchange(false)) {
        return;
    }

    log(LogLevel::info, "Gedouble-Core runtime shut down");
}

bool is_initialized() noexcept {
    return g_initialized.load();
}

void set_log_callback(LogCallback callback) noexcept {
    std::lock_guard<std::mutex> lock(g_log_mutex);
    g_log_callback = callback;
}

void log(LogLevel level, const char* message) noexcept {
    if (message == nullptr) {
        return;
    }

    LogCallback callback = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_log_mutex);
        callback = g_log_callback;
    }

    if (callback != nullptr) {
        callback(level, message);
    }
}

} // namespace gedouble
