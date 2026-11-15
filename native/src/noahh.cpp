#include "gedouble/noahh.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <system_error>
#include <vector>

namespace gedouble {
namespace {

void set_error(char* out, std::size_t size, const char* message) noexcept {
    if (out == nullptr || size == 0) return;
    std::strncpy(out, message, size - 1);
    out[size - 1] = '\0';
}

bool copy_text(char* out, std::size_t capacity, const std::string& value) {
    if (value.size() >= capacity) return false;
    std::memcpy(out, value.c_str(), value.size() + 1);
    return true;
}

class JsonReader {
public:
    explicit JsonReader(const std::string& source) : source_(source) {}

    bool parse(NoahhModMetadata& result) {
        skip_space();
        if (!take('{')) return fail("mod.json must contain a JSON object");

        bool has_id = false, has_name = false, has_version = false;
        bool has_noahh = false, has_android = false, has_developer = false;

        skip_space();
        while (!take('}')) {
            std::string key;
            if (!string(key)) return fail("expected a JSON property name");
            skip_space();
            if (!take(':')) return fail("expected ':' after property name");
            skip_space();

            if (key == "id" || key == "name" || key == "version"
                || key == "noahh" || key == "developer" || key == "description") {
                std::string value;
                if (!string(value)) return fail("expected a string for mod metadata");
                bool copied = true;
                if (key == "id") { copied = copy_text(result.id, sizeof(result.id), value); has_id = copied; }
                else if (key == "name") { copied = copy_text(result.name, sizeof(result.name), value); has_name = copied; }
                else if (key == "version") { copied = copy_text(result.version, sizeof(result.version), value); has_version = copied; }
                else if (key == "noahh") { copied = copy_text(result.noahh_version, sizeof(result.noahh_version), value); has_noahh = copied; }
                else if (key == "developer") { copied = copy_text(result.developer, sizeof(result.developer), value); has_developer = copied; }
                else copied = copy_text(result.description, sizeof(result.description), value);
                if (!copied) return fail("a mod metadata field exceeds its supported length");
            } else if (key == "developers") {
                std::string developer;
                if (!string_array_first(developer)) return fail("developers must be an array of strings");
                if (!developer.empty()) {
                    if (!copy_text(result.developer, sizeof(result.developer), developer))
                        return fail("developer name is too long");
                    has_developer = true;
                }
            } else if (key == "gd") {
                if (!parse_game_versions(result, has_android)) return fail("gd must contain an android version string");
            } else if (!skip_value()) {
                return fail("invalid JSON value");
            }

            skip_space();
            if (take('}')) break;
            if (!take(',')) return fail("expected ',' or '}'");
            skip_space();
        }

        skip_space();
        if (position_ != source_.size()) return fail("unexpected content after mod.json");
        if (!has_id || !has_name || !has_version || !has_noahh || !has_developer || !has_android)
            return fail("required Noahh fields: noahh, gd.android, id, name, version and developer(s)");
        if (!valid_id(result.id)) return fail("invalid Noahh mod id");
        return true;
    }

    const std::string& error() const { return error_; }

private:
    bool fail(const char* message) { error_ = message; return false; }

    void skip_space() {
        while (position_ < source_.size() && std::isspace(static_cast<unsigned char>(source_[position_]))) ++position_;
    }

    bool take(char c) {
        if (position_ < source_.size() && source_[position_] == c) { ++position_; return true; }
        return false;
    }

    bool string(std::string& out) {
        if (!take('"')) return false;
        out.clear();
        while (position_ < source_.size()) {
            const char c = source_[position_++];
            if (c == '"') return true;
            if (static_cast<unsigned char>(c) < 0x20) return false;
            if (c == '\\') {
                if (position_ >= source_.size()) return false;
                const char escaped = source_[position_++];
                switch (escaped) {
                    case '"': out.push_back('"'); break;
                    case '\\': out.push_back('\\'); break;
                    case '/': out.push_back('/'); break;
                    case 'b': out.push_back('\b'); break;
                    case 'f': out.push_back('\f'); break;
                    case 'n': out.push_back('\n'); break;
                    case 'r': out.push_back('\r'); break;
                    case 't': out.push_back('\t'); break;
                    default: return false;
                }
            } else out.push_back(c);
            if (out.size() > 4096) return false;
        }
        return false;
    }

    bool string_array_first(std::string& first) {
        if (!take('[')) return false;
        skip_space();
        if (take(']')) return true;
        bool first_item = true;
        while (true) {
            std::string value;
            if (!string(value)) return false;
            if (first_item) { first = value; first_item = false; }
            skip_space();
            if (take(']')) return true;
            if (!take(',')) return false;
            skip_space();
        }
    }

    bool parse_game_versions(NoahhModMetadata& result, bool& has_android) {
        if (!take('{')) return false;
        skip_space();
        while (!take('}')) {
            std::string platform;
            if (!string(platform)) return false;
            skip_space();
            if (!take(':')) return false;
            skip_space();
            if (platform == "android") {
                std::string version;
                if (!string(version) || !copy_text(result.android_game_version,
                    sizeof(result.android_game_version), version)) return false;
                has_android = true;
            } else if (!skip_value()) return false;
            skip_space();
            if (take('}')) break;
            if (!take(',')) return false;
            skip_space();
        }
        return true;
    }

    bool skip_value() {
        if (position_ >= source_.size()) return false;
        if (source_[position_] == '"') { std::string ignored; return string(ignored); }
        if (source_[position_] == '{' || source_[position_] == '[') {
            const char close = source_[position_++] == '{' ? '}' : ']';
            skip_space();
            if (take(close)) return true;
            while (position_ < source_.size()) {
                if (source_[position_] == '"' ) { std::string ignored; if (!string(ignored)) return false; }
                else if (source_[position_] == '{' || source_[position_] == '[') { if (!skip_value()) return false; }
                else {
                    while (position_ < source_.size() && source_[position_] != ','
                        && source_[position_] != ']' && source_[position_] != '}') ++position_;
                }
                skip_space();
                if (take(close)) return true;
                if (!take(',')) return false;
                skip_space();
            }
            return false;
        }
        const auto start = position_;
        while (position_ < source_.size() && source_[position_] != ','
            && source_[position_] != '}' && source_[position_] != ']'
            && !std::isspace(static_cast<unsigned char>(source_[position_]))) ++position_;
        return position_ > start;
    }

    static bool valid_id(const char* id) {
        if (*id == '\0') return false;
        for (const unsigned char* p = reinterpret_cast<const unsigned char*>(id); *p; ++p)
            if (!( (*p >= 'a' && *p <= 'z') || *p == '.' || *p == '_' || *p == '-')) return false;
        return true;
    }

    const std::string& source_;
    std::size_t position_ = 0;
    std::string error_;
};

} // namespace

bool read_noahh_mod_json(const char* path, NoahhModMetadata* output,
                         char* error_buffer, std::size_t error_buffer_size) noexcept {
    if (path == nullptr || output == nullptr) {
        set_error(error_buffer, error_buffer_size, "invalid argument");
        return false;
    }
    try {
        std::ifstream file(path, std::ios::binary);
        if (!file) { set_error(error_buffer, error_buffer_size, "cannot open mod.json"); return false; }
        std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if (contents.size() > 64 * 1024) { set_error(error_buffer, error_buffer_size, "mod.json exceeds 64 KiB"); return false; }
        NoahhModMetadata parsed{};
        JsonReader reader(contents);
        if (!reader.parse(parsed)) { set_error(error_buffer, error_buffer_size, reader.error().c_str()); return false; }
        *output = parsed;
        set_error(error_buffer, error_buffer_size, "");
        return true;
    } catch (...) {
        set_error(error_buffer, error_buffer_size, "unexpected error reading mod.json");
        return false;
    }
}

bool scan_noahh_packages(const char* directory, NoahhPackageInfo* output,
                         std::size_t capacity, std::size_t* package_count) noexcept {
    if (package_count != nullptr) *package_count = 0;
    if (directory == nullptr || package_count == nullptr || (output == nullptr && capacity != 0)) return false;
    try {
        std::error_code ec;
        const std::filesystem::path root(directory);
        if (!std::filesystem::is_directory(root, ec) || ec) return false;
        std::size_t found = 0;
        for (std::filesystem::directory_iterator it(root, ec), end; it != end && !ec; it.increment(ec)) {
            std::error_code item_error;
            if (!it->is_regular_file(item_error) || item_error) continue;
            if (it->path().extension() != ".noahh") continue;
            if (found < capacity && output != nullptr) {
                NoahhPackageInfo info{};
                const auto filename = it->path().filename().string();
                const auto size = it->file_size(item_error);
                if (item_error || !copy_text(info.filename, sizeof(info.filename), filename)) continue;
                info.size_bytes = static_cast<std::uint64_t>(size);
                output[found] = info;
            }
            ++found;
        }
        *package_count = std::min(found, capacity);
        return !ec;
    } catch (...) { return false; }
}

const char* noahh_android_mods_relative_path() noexcept {
    return "Android/media/com.noahh.launcher/game/noahh/mods";
}

} // namespace gedouble
