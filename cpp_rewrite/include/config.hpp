#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

class Config {
public:
    struct JsonConfig {
        std::string rl_install_path;
        std::vector<std::string> custom_maps;
        std::optional<std::string> loaded_map;
    };

    Config();
    ~Config() = default;

    // Getters
    fs::path get_rl_install_path() const;
    std::vector<std::string> get_custom_maps() const;
    std::optional<fs::path> get_loaded_map() const;
    fs::path get_maps_dir() const;
    bool is_valid_rl_install() const;

    // Setters
    void set_rl_install_path(const fs::path& path);
    void add_custom_map(const std::string& map_path);
    void remove_custom_map(const std::string& map_path);

    // Map operations
    std::pair<bool, std::string> load_map(const fs::path& map_file);
    std::pair<bool, std::string> restore_original();

    // Persistence
    void save() const;

private:
    fs::path config_file;
    fs::path maps_dir;
    JsonConfig data;

    JsonConfig load_config();
    fs::path get_data_dir() const;
    fs::path get_default_rl_path() const;
    
    // Backwards compatibility
    void migrate_config();
    void migrate_backup_format();
};
