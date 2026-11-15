#include "gedouble/manifest.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <system_error>

namespace gedouble {
namespace {

void set_error(char* buffer, std::size_t size, const char* message) noexcept {
    if (buffer == nullptr || size == 0) return;
    std::strncpy(buffer, message, size - 1);
    buffer[size - 1] = '\0';
}

bool copy_field(char* destination, std::size_t capacity, const std::string& value) {
    if (value.size() >= capacity) return false;
    std::memcpy(destination, value.c_str(), value.size() + 1);
    return true;
}

class JsonManifestParser {
public:
    explicit JsonManifestParser(const std::string& input) : input_(input) {}

    bool parse(ModManifest& manifest) {
        skip_space();
        if (!consume('{')) return fail("manifest must be a JSON object");

        bool has_id = false, has_name = false, has_version = false, has_entry = false;
        skip_space();
        while (!consume('}')) {
            std::string key;
            if (!parse_string(key)) return fail("expected a JSON property name");
            skip_space();
            if (!consume(':')) return fail("expected ':' after property name");
            skip_space();

            if (key == "id" || key == "name" || key == "version"
                || key == "entry" || key == "description") {
                std::string value;
                if (!parse_string(value)) return fail("manifest fields must be strings");
                if (key == "id") has_id = copy_field(manifest.id, sizeof(manifest.id), value);
                else if (key == "name") has_name = copy_field(manifest.name, sizeof(manifest.name), value);
                else if (key == "version") has_version = copy_field(manifest.version, sizeof(manifest.version), value);
                else if (key == "entry") has_entry = copy_field(manifest.entry, sizeof(manifest.entry), value);
                else if (!copy_field(manifest.description, sizeof(manifest.description), value))
                    return fail("description is too long");
                if ((key == "id" && !has_id) || (key == "name" && !has_name)
                    || (key == "version" && !has_version) || (key == "entry" && !has_entry))
                    return fail("a manifest field is too long");
            } else if (!skip_value()) {
                return fail("invalid JSON value");
            }

            skip_space();
            if (consume('}')) break;
            if (!consume(',')) return fail("expected ',' or '}'");
            skip_space();
        }

        skip_space();
        if (position_ != input_.size()) return fail("unexpected data after JSON object");
        if (!has_id || !has_name || !has_version || !has_entry)
            return fail("required fields: id, name, version, entry");
        if (!valid_id(manifest.id)) return fail("invalid mod id");
        if (manifest.name[0] == '\0' || manifest.version[0] == '\0')
            return fail("name and version cannot be empty");
        if (!valid_relative_entry(manifest.entry))
            return fail("entry must be a safe relative path");
        return true;
    }

    const std::string& error() const { return error_; }

private:
    bool fail(const char* message) { error_ = message; return false; }

    void skip_space() {
        while (position_ < input_.size()
            && std::isspace(static_cast<unsigned char>(input_[position_]))) ++position_;
    }

    bool consume(char c) {
        if (position_ < input_.size() && input_[position_] == c) {
            ++position_;
            return true;
        }
        return false;
    }

    bool parse_string(std::string& output) {
        if (!consume('"')) return false;
        output.clear();
        while (position_ < input_.size()) {
            char c = input_[position_++];
            if (c == '"') return true;
            if (static_cast<unsigned char>(c) < 0x20) return false;
            if (c == '\\') {
                if (position_ >= input_.size()) return false;
                const char escaped = input_[position_++];
                switch (escaped) {
                    case '"': output.push_back('"'); break;
                    case '\\': output.push_back('\\'); break;
                    case '/': output.push_back('/'); break;
                    case 'b': output.push_back('\b'); break;
                    case 'f': output.push_back('\f'); break;
                    case 'n': output.push_back('\n'); break;
                    case 'r': output.push_back('\r'); break;
                    case 't': output.push_back('\t'); break;
                    default: return false;
                }
            } else {
                output.push_back(c);
            }
            if (output.size() > 4096) return false;
        }
        return false;
    }

    bool skip_value() {
        if (position_ >= input_.size()) return false;
        if (input_[position_] == '"') {
            std::string ignored;
            return parse_string(ignored);
        }
        if (input_[position_] == '{' || input_[position_] == '[') {
            const char open = input_[position_++];
            const char close = open == '{' ? '}' : ']';
            skip_space();
            if (consume(close)) return true;
            while (position_ < input_.size()) {
                if (input_[position_] == '"' ) {
                    std::string ignored;
                    if (!parse_string(ignored)) return false;
                } else if (input_[position_] == '{' || input_[position_] == '[') {
                    if (!skip_value()) return false;
                } else {
                    while (position_ < input_.size() && input_[position_] != ','
                        && input_[position_] != ']' && input_[position_] != '}') ++position_;
                }
                skip_space();
                if (consume(close)) return true;
                if (!consume(',')) return false;
                skip_space();
            }
            return false;
        }
        const auto start = position_;
        while (position_ < input_.size() && input_[position_] != ','
            && input_[position_] != '}' && input_[position_] != ']'
            && !std::isspace(static_cast<unsigned char>(input_[position_]))) ++position_;
        return position_ > start;
    }

    static bool valid_id(const char* id) {
        if (id[0] == '\0') return false;
        for (const unsigned char* p = reinterpret_cast<const unsigned char*>(id); *p; ++p) {
            if (!(std::isalnum(*p) || *p == '.' || *p == '_' || *p == '-')) return false;
        }
        return true;
    }

    static bool valid_relative_entry(const char* entry) {
        if (entry[0] == '\0' || entry[0] == '/' || entry[0] == '\\') return false;
        const std::filesystem::path path(entry);
        if (path.is_absolute()) return false;
        for (const auto& part : path) {
            if (part == ".." || part == ".") return false;
        }
        return true;
    }

    const std::string& input_;
    std::size_t position_ = 0;
    std::string error_;
};

} // namespace

bool read_mod_manifest(const char* manifest_path, ModManifest* output,
                       char* error_buffer, std::size_t error_buffer_size) noexcept {
    if (manifest_path == nullptr || output == nullptr) {
        set_error(error_buffer, error_buffer_size, "invalid argument");
        return false;
    }

    try {
        std::ifstream file(manifest_path, std::ios::binary);
        if (!file) {
            set_error(error_buffer, error_buffer_size, "cannot open manifest");
            return false;
        }
        std::string contents((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        if (contents.size() > 64 * 1024) {
            set_error(error_buffer, error_buffer_size, "manifest exceeds 64 KiB");
            return false;
        }

        ModManifest parsed{};
        JsonManifestParser parser(contents);
        if (!parser.parse(parsed)) {
            set_error(error_buffer, error_buffer_size, parser.error().c_str());
            return false;
        }
        *output = parsed;
        set_error(error_buffer, error_buffer_size, "");
        return true;
    } catch (...) {
        set_error(error_buffer, error_buffer_size, "unexpected error reading manifest");
        return false;
    }
}

bool discover_mods(const char* root_path, ModManifest* output,
                   std::size_t output_capacity, std::size_t* discovered_count) noexcept {
    if (discovered_count != nullptr) *discovered_count = 0;
    if (root_path == nullptr || discovered_count == nullptr
        || (output == nullptr && output_capacity != 0)) return false;

    try {
        const std::filesystem::path root(root_path);
        std::error_code ec;
        if (!std::filesystem::is_directory(root, ec) || ec) return false;

        std::size_t count = 0;
        for (std::filesystem::directory_iterator it(root, ec), end; it != end && !ec; it.increment(ec)) {
            std::error_code type_error;
            if (!it->is_directory(type_error) || type_error) continue;

            const auto manifest_path = it->path() / "manifest.json";
            if (!std::filesystem::is_regular_file(manifest_path, type_error) || type_error) continue;

            ModManifest manifest{};
            char error[160]{};
            if (!read_mod_manifest(manifest_path.string().c_str(), &manifest, error, sizeof(error))) {
                log(LogLevel::warning, error);
                continue;
            }

            if (count < output_capacity && output != nullptr) output[count] = manifest;
            ++count;
        }

        *discovered_count = std::min(count, output_capacity);
        return !ec;
    } catch (...) {
        return false;
    }
}

} // namespace gedouble
