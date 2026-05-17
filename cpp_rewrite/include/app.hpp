#pragma once

#include "config.hpp"
#include <imgui.h>
#include <string>
#include <chrono>

class RLMapLoaderApp {
public:
    RLMapLoaderApp();
    ~RLMapLoaderApp();

    bool init();
    void run();
    bool should_close() const;

private:
    Config config;
    std::string rl_path_input;
    int selected_map_index;
    std::string status_message;
    ImVec4 status_color;
    std::chrono::system_clock::time_point status_time;

    // Rendering
    void render();
    void render_header();
    void render_map_list();
    void render_status();

    // Event handling
    void handle_dropped_files(const std::vector<std::string>& file_paths);
    void confirm_rl_install_path();
    void set_status(const std::string& message, bool success = true);

    // Window
    struct GLFWwindow* window;
    class ImGui_ImplGlfw_Data* impl;
};
