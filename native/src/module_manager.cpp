#include "gedouble/module.hpp"
#include "module_manager_internal.hpp"

#include <mutex>
#include <string>
#include <vector>

namespace gedouble {
namespace {
struct LoadedModule {
    std::string id;
    std::string version;
    void (*on_unload)() noexcept = nullptr;
};

std::mutex g_modules_mutex;
std::vector<LoadedModule> g_modules;
}

bool register_module(const ModuleDescriptor& descriptor) noexcept {
    if (descriptor.id == nullptr || descriptor.id[0] == '\0'
        || descriptor.version == nullptr) {
        return false;
    }

    std::lock_guard<std::mutex> lock(g_modules_mutex);
    if (!is_initialized()) {
        return false;
    }

    for (const auto& module : g_modules) {
        if (module.id == descriptor.id) {
            return false;
        }
    }

    if (descriptor.on_load != nullptr && !descriptor.on_load()) {
        log(LogLevel::warning, "A module rejected its load request");
        return false;
    }

    try {
        g_modules.push_back({descriptor.id, descriptor.version, descriptor.on_unload});
    } catch (...) {
        if (descriptor.on_unload != nullptr) {
            descriptor.on_unload();
        }
        log(LogLevel::error, "Unable to allocate module registry entry");
        return false;
    }

    log(LogLevel::info, "Native module registered");
    return true;
}

bool unregister_module(const char* id) noexcept {
    if (id == nullptr) {
        return false;
    }

    void (*unload)() noexcept = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_modules_mutex);
        for (auto it = g_modules.begin(); it != g_modules.end(); ++it) {
            if (it->id == id) {
                unload = it->on_unload;
                g_modules.erase(it);
                break;
            }
        }
    }

    if (unload == nullptr) {
        return false;
    }
    unload();
    log(LogLevel::info, "Native module unregistered");
    return true;
}

std::size_t loaded_module_count() noexcept {
    std::lock_guard<std::mutex> lock(g_modules_mutex);
    return g_modules.size();
}

bool is_module_loaded(const char* id) noexcept {
    if (id == nullptr) {
        return false;
    }

    std::lock_guard<std::mutex> lock(g_modules_mutex);
    for (const auto& module : g_modules) {
        if (module.id == id) {
            return true;
        }
    }
    return false;
}

namespace detail {
void unload_all_modules() noexcept {
    std::vector<LoadedModule> modules;
    {
        std::lock_guard<std::mutex> lock(g_modules_mutex);
        modules.swap(g_modules);
    }

    for (auto it = modules.rbegin(); it != modules.rend(); ++it) {
        if (it->on_unload != nullptr) {
            it->on_unload();
        }
    }
}
} // namespace detail
} // namespace gedouble
