#pragma once

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class MapManager {
public:
    static constexpr std::string_view TARGET_MAP = "Labs_Underpass_P.upk";
    static constexpr std::string_view BACKUP_EXT = ".bak";

    explicit MapManager(const fs::path& rl_install_path);
    ~MapManager() = default;

    bool is_valid_rl_install() const;
    bool has_backup() const;
    bool has_custom_map_installed() const;

    std::pair<bool, std::string> load_map(const fs::path& map_file);
    std::pair<bool, std::string> restore_original();

private:
    fs::path rl_install_path;
    fs::path cooked_dir;
    fs::path target_file;
    fs::path backup_file;

    void migrate_backup_format();
};
