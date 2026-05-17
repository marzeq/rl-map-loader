#pragma once

#include "config.hpp"
#include <string>
#include <chrono>
#include <vector>
#include <array>

struct GLFWwindow;

class RLMapLoaderApp {
public:
    RLMapLoaderApp();
    ~RLMapLoaderApp();

    bool init();
    void run();
    bool should_close() const;
    void handle_dropped_files(const std::vector<std::string>& file_paths);

private:
    Config config;
    std::string rl_path_input;
    int selected_map_index;
    std::string status_message;
    std::array<float, 4> status_color;  // RGBA: red, green, blue, alpha
    std::chrono::system_clock::time_point status_time;

    // Rendering
    void render();
    void render_header();
    void render_map_list();
    void render_status();

    // Event handling
    void confirm_rl_install_path();
    void set_status(const std::string& message, bool success = true);

    // Window
    GLFWwindow* window;
    void* impl;
};
