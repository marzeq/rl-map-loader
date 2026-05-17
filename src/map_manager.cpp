#include "map_manager.hpp"

#include <iostream>

namespace fs = std::filesystem;

MapManager::MapManager(const fs::path& rl_install_path)
    : rl_install_path(rl_install_path) {
    cooked_dir = this->rl_install_path / "TAGame" / "CookedPCConsole";
    target_file = cooked_dir / std::string(TARGET_MAP);
    backup_file = cooked_dir / (std::string(TARGET_MAP) + std::string(BACKUP_EXT));
    migrate_backup_format();
}

bool MapManager::is_valid_rl_install() const {
    return fs::exists(cooked_dir) && fs::is_directory(cooked_dir);
}

bool MapManager::has_backup() const {
    // Migration from backup file being .upk.upk.bak to .upk.bak
    fs::path old_backup = cooked_dir / (std::string(TARGET_MAP) + ".upk.bak");
    
    if (fs::exists(old_backup)) {
        try {
            fs::rename(old_backup, backup_file);
        } catch (const std::exception& e) {
            std::cerr << "Failed to migrate backup: " << e.what() << '\n';
        }
    }

    return fs::exists(backup_file);
}

bool MapManager::has_custom_map_installed() const {
    return has_backup();
}

std::pair<bool, std::string> MapManager::load_map(const fs::path& map_file) {
    auto map_file_path = fs::absolute(map_file);

    if (!fs::exists(map_file_path)) {
        return {false, "Map file not found: " + map_file_path.string()};
    }

    if (map_file_path.extension().string() != ".upk") {
        return {false, "Invalid map file extension: " + map_file_path.extension().string()};
    }

    if (!fs::exists(cooked_dir)) {
        try {
            fs::create_directories(cooked_dir);
        } catch (const std::exception& e) {
            return {false, std::string("Failed to create directory: ") + e.what()};
        }
    }

    if (!has_backup() && fs::exists(target_file)) {
        try {
            fs::copy_file(target_file, backup_file);
        } catch (const std::exception& e) {
            return {false, std::string("Failed to create backup: ") + e.what()};
        }
    }

    try {
        fs::copy_file(map_file_path, target_file, fs::copy_options::overwrite_existing);
        return {true, std::string("Successfully loaded map: ") + map_file_path.filename().string()};
    } catch (const std::exception& e) {
        return {false, std::string("Failed to load map: ") + e.what()};
    }
}

std::pair<bool, std::string> MapManager::restore_original() {
    if (!has_backup()) {
        return {false, "No backup found"};
    }

    try {
        fs::copy_file(backup_file, target_file, fs::copy_options::overwrite_existing);
        fs::remove(backup_file);
        return {true, "Successfully restored original map"};
    } catch (const std::exception& e) {
        return {false, std::string("Failed to restore map: ") + e.what()};
    }
}

void MapManager::migrate_backup_format() {
    // This is called automatically in constructor
    has_backup();
}
