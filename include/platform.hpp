#pragma once

#include <filesystem>
#include <string_view>

namespace fs = std::filesystem;

namespace platform {
    fs::path get_data_dir();
    fs::path get_default_rl_path();
    std::string_view get_os_name();
}
