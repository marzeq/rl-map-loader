#include "platform.hpp"

#include <cstdlib>

#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#else
#include <pwd.h>
#include <unistd.h>
#endif

namespace fs = std::filesystem;

fs::path platform::get_data_dir() {
    fs::path data_dir;

#ifdef _WIN32
    char app_data[MAX_PATH] = {0};
    if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, app_data))) {
        data_dir = fs::path(app_data) / "rl-map-loader";
    } else {
        data_dir = fs::path(getenv("APPDATA")) / "rl-map-loader";
    }
#else
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = pw->pw_dir;
    }
    data_dir = fs::path(home) / ".local" / "share" / "rl-map-loader";
#endif

    fs::create_directories(data_dir);
    return data_dir;
}

fs::path platform::get_default_rl_path() {
#ifdef _WIN32
    return fs::path(R"(C:\Program Files\Epic Games\rocketleague)");
#else
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = pw->pw_dir;
    }
    return fs::path(home) / "Games" / "Heroic" / "rocketleague";
#endif
}

std::string_view platform::get_os_name() {
#ifdef _WIN32
    return "windows";
#else
    return "linux";
#endif
}
