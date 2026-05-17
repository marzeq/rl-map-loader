#include "app.hpp"
#include "config.hpp"
#include "platform.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>
#include <algorithm>

namespace fs = std::filesystem;

// Helper function for file drop callback
static void drop_callback(GLFWwindow* window, int count, const char** paths) {
    RLMapLoaderApp* app = static_cast<RLMapLoaderApp*>(glfwGetWindowUserPointer(window));
    if (app) {
        std::vector<std::string> file_paths;
        for (int i = 0; i < count; ++i) {
            file_paths.push_back(paths[i]);
        }
        app->handle_dropped_files(file_paths);
    }
}

RLMapLoaderApp::RLMapLoaderApp()
    : rl_path_input(config.get_rl_install_path().string()),
      selected_map_index(-1),
      status_color({1.0f, 1.0f, 1.0f, 1.0f}),
      window(nullptr),
      impl(nullptr) {
}

RLMapLoaderApp::~RLMapLoaderApp() {
    if (impl) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    if (window) {
        glfwDestroyWindow(window);
    }

    glfwTerminate();
}

bool RLMapLoaderApp::init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    const char* glsl_version = "#version 150";

#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    window = glfwCreateWindow(1000, 700, "RL Map Loader", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetWindowUserPointer(window, this);
    glfwSetDropCallback(window, drop_callback);

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    impl = window;  // Store window pointer for later use

    return true;
}

void RLMapLoaderApp::run() {
    if (!init()) {
        return;
    }

    while (!should_close()) {
        glfwPollEvents();

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        render();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}

bool RLMapLoaderApp::should_close() const {
    return glfwWindowShouldClose(window);
}

void RLMapLoaderApp::render() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("RL Map Loader", nullptr,
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    render_header();
    if (config.is_valid_rl_install()) {
        render_map_list();
    }
    render_status();

    ImGui::End();
}

void RLMapLoaderApp::render_header() {
    ImGui::Text("Rocket League Installation Path:");

    static char path_buffer[512] = {};
    if (rl_path_input.size() < sizeof(path_buffer)) {
        std::copy(rl_path_input.begin(), rl_path_input.end(), path_buffer);
    }
    ImGui::InputText("##rl_path", path_buffer, sizeof(path_buffer));
    rl_path_input = path_buffer;

    if (ImGui::Button("Confirm")) {
        confirm_rl_install_path();
    }

    ImGui::SameLine();
    if (ImGui::Button("Reset to Default")) {
        auto default_path = platform::get_default_rl_path();
        rl_path_input = default_path.string();
        config.set_rl_install_path(default_path);
        set_status("Reset to default path", true);
    }

    if (!config.is_valid_rl_install()) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                          "Invalid Rocket League installation path");
    }

    ImGui::Separator();
}

void RLMapLoaderApp::render_map_list() {
    ImGui::Text("Drag & drop .udk or .upk to add maps");

    auto custom_maps = config.get_custom_maps();

    if (custom_maps.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No custom maps added");
    } else {
        ImGui::Text("Available maps:");
        ImGui::BeginChild("maps_list", ImVec2(0, 200), true);

        for (int i = 0; i < static_cast<int>(custom_maps.size()); ++i) {
            bool selected = selected_map_index == i;
            auto map_path = fs::path(custom_maps[i]);
            std::string label = map_path.stem().string();

            if (ImGui::Selectable(label.c_str(), selected)) {
                selected_map_index = i;
            }
        }

        ImGui::EndChild();
    }

    custom_maps = config.get_custom_maps();
    bool has_selection = selected_map_index >= 0 && selected_map_index < static_cast<int>(custom_maps.size());

    if (ImGui::Button("Load Selected Map", ImVec2(200, 0))) {
        if (has_selection) {
            auto map_path = fs::path(custom_maps[selected_map_index]);
            auto [success, message] = config.load_map(map_path);
            set_status(message, success);
        } else {
            set_status("No map selected", false);
        }
    }

    ImGui::SameLine();

    bool restore_enabled = config.get_loaded_map().has_value();
    if (!restore_enabled) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }

    if (ImGui::Button("Restore Original", ImVec2(200, 0))) {
        if (restore_enabled) {
            auto [success, message] = config.restore_original();
            set_status(message, success);
            selected_map_index = -1;
        }
    }

    if (!custom_maps.empty()) {
        ImGui::SameLine();
        if (ImGui::Button("Remove Selected", ImVec2(200, 0))) {
            if (has_selection) {
                config.remove_custom_map(custom_maps[selected_map_index]);
                selected_map_index = -1;
                set_status("Map removed", true);
            } else {
                set_status("No map selected", false);
            }
        }
    }

    if (!restore_enabled) {
        ImGui::PopStyleVar();
    }

    auto loaded_map = config.get_loaded_map();
    if (loaded_map) {
        auto loaded_map_name = loaded_map->stem().string();
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
                          "Custom map \"%s\" is active. Launch freeplay map \"Underpass - Soccar\" to play it",
                          loaded_map_name.c_str());
    } else {
        ImGui::Text("Original map is active");
    }
}

void RLMapLoaderApp::render_status() {
    if (!status_message.empty()) {
        auto elapsed = std::chrono::system_clock::now() - status_time;
        if (elapsed < std::chrono::seconds(5)) {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(status_color[0], status_color[1], status_color[2], status_color[3]),
                              "%s", status_message.c_str());
        } else {
            status_message.clear();
        }
    }
}

void RLMapLoaderApp::confirm_rl_install_path() {
    auto path = fs::path(rl_path_input);
    config.set_rl_install_path(path);
    set_status("Updated Rocket League installation path", true);
}

void RLMapLoaderApp::set_status(const std::string& message, bool success) {
    status_message = message;
    if (success) {
        status_color = {0.0f, 1.0f, 0.0f, 1.0f};  // Green
    } else {
        status_color = {1.0f, 0.0f, 0.0f, 1.0f};  // Red
    }
    status_time = std::chrono::system_clock::now();
}

void RLMapLoaderApp::handle_dropped_files(const std::vector<std::string>& file_paths) {
    std::vector<fs::path> okay_files;
    std::vector<std::string> not_okay_files;

    for (const auto& file_path : file_paths) {
        fs::path path(file_path);
        auto ext = path.extension().string();

        // Convert to lowercase for comparison
        std::transform(ext.begin(), ext.end(), ext.begin(),
                      [](unsigned char c) { return std::tolower(c); });

        if (ext != ".udk" && ext != ".upk") {
            not_okay_files.push_back(path.filename().string());
            continue;
        }
        okay_files.push_back(path);
    }

    if (!not_okay_files.empty()) {
        std::string files_str = not_okay_files[0];
        for (size_t i = 1; i < not_okay_files.size(); ++i) {
            files_str += ", " + not_okay_files[i];
        }
        set_status("Invalid map files: " + files_str, false);
        return;
    }

    for (const auto& path : okay_files) {
        try {
            auto target_name = path.stem().string() + ".upk";
            auto target_path = config.get_maps_dir() / target_name;

            fs::copy_file(path, target_path, fs::copy_options::overwrite_existing);
            config.add_custom_map(target_path.string());
            set_status("Added map: " + target_name, true);
        } catch (const std::exception& e) {
            set_status(std::string("Failed to add map: ") + e.what(), false);
        }
    }
}
