#include "config.hpp"
#include "map_manager.hpp"
#include "platform.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <optional>
#include <algorithm>

using json = nlohmann::json;
namespace fs = std::filesystem;

Config::Config() {
    config_file = platform::get_data_dir() / "config.json";
    maps_dir = platform::get_data_dir() / "maps";
    fs::create_directories(maps_dir);

    data = load_config();
    auto rl_install_path = fs::path(data.rl_install_path);

    // Ensure MapManager is created and performs migrations
    auto map_manager = MapManager(rl_install_path);

    // Migrate config if needed
    migrate_config();
}

Config::JsonConfig Config::load_config() {
    if (fs::exists(config_file)) {
        try {
            std::ifstream file(config_file);
            json j = json::parse(file);

            JsonConfig cfg;
            cfg.rl_install_path = j.value("rl_install_path", platform::get_default_rl_path().string());
            cfg.custom_maps = j.value("custom_maps", std::vector<std::string>{});
            
            if (j.contains("loaded_map") && !j["loaded_map"].is_null()) {
                cfg.loaded_map = j["loaded_map"].get<std::string>();
            } else {
                cfg.loaded_map = std::nullopt;
            }

            return cfg;
        } catch (const std::exception& e) {
            std::cerr << "Failed to parse config.json: " << e.what() << '\n';
            // Fall through to default config
        }
    }

    JsonConfig cfg;
    cfg.rl_install_path = platform::get_default_rl_path().string();
    cfg.custom_maps = {};
    cfg.loaded_map = std::nullopt;
    return cfg;
}

void Config::save() const {
    try {
        json j;
        j["rl_install_path"] = data.rl_install_path;
        j["custom_maps"] = data.custom_maps;
        j["loaded_map"] = data.loaded_map;

        std::ofstream file(config_file);
        file << j.dump(2);
    } catch (const std::exception& e) {
        std::cerr << "Failed to save config: " << e.what() << '\n';
    }
}

fs::path Config::get_rl_install_path() const {
    return fs::path(data.rl_install_path);
}

std::vector<std::string> Config::get_custom_maps() const {
    return data.custom_maps;
}

std::optional<fs::path> Config::get_loaded_map() const {
    if (data.loaded_map) {
        return fs::path(*data.loaded_map);
    }
    return std::nullopt;
}

fs::path Config::get_maps_dir() const {
    return maps_dir;
}

bool Config::is_valid_rl_install() const {
    MapManager mm(get_rl_install_path());
    return mm.is_valid_rl_install();
}

void Config::set_rl_install_path(const fs::path& path) {
    data.rl_install_path = fs::absolute(path).string();
    save();
}

void Config::add_custom_map(const std::string& map_path) {
    auto abs_path = fs::absolute(map_path).string();
    auto it = std::find(data.custom_maps.begin(), data.custom_maps.end(), abs_path);
    if (it == data.custom_maps.end()) {
        data.custom_maps.push_back(abs_path);
        save();
    }
}

void Config::remove_custom_map(const std::string& map_path) {
    auto map_path_obj = fs::absolute(map_path);
    auto map_path_str = map_path_obj.string();

    auto it = std::find(data.custom_maps.begin(), data.custom_maps.end(), map_path_str);
    if (it != data.custom_maps.end()) {
        data.custom_maps.erase(it);
        save();

        // Delete the file if it's in our maps directory
        if (fs::exists(map_path_obj) && map_path_obj.parent_path() == maps_dir) {
            try {
                fs::remove(map_path_obj);
            } catch (const std::exception& e) {
                std::cerr << "Failed to delete map file: " << e.what() << '\n';
            }
        }
    }

    // If this was the loaded map, restore the original
    auto loaded = get_loaded_map();
    if (loaded && fs::absolute(*loaded) == map_path_obj) {
        restore_original();
    }
}

std::pair<bool, std::string> Config::load_map(const fs::path& map_file) {
    MapManager mm(get_rl_install_path());
    auto [success, message] = mm.load_map(map_file);
    
    if (success) {
        data.loaded_map = fs::absolute(map_file).string();
        save();
    }
    
    return {success, message};
}

std::pair<bool, std::string> Config::restore_original() {
    MapManager mm(get_rl_install_path());
    auto [success, message] = mm.restore_original();
    
    if (success) {
        data.loaded_map = std::nullopt;
        save();
    }
    
    return {success, message};
}

fs::path Config::get_data_dir() const {
    return platform::get_data_dir();
}

fs::path Config::get_default_rl_path() const {
    return platform::get_default_rl_path();
}

void Config::migrate_config() {
    MapManager mm(get_rl_install_path());
    
    // Migration: check if backup exists but loaded_map is none
    // if so, move backup back into original location and clear backup
    if (mm.has_backup() && !data.loaded_map) {
        mm.restore_original();
        data.loaded_map = std::nullopt;
        save();
    }
}

void Config::migrate_backup_format() {
    // No-op: migrations are handled in MapManager
}
