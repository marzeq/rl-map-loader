"""Main application for RL Map Loader using Dear ImGui."""

from pathlib import Path

import imgui
from imgui.integrations.glfw import GlfwRenderer
import glfw

from .config import Config, get_default_rl_path
from .map_manager import MapManager


class RLMapLoaderApp:
    """Main application class for RL Map Loader."""

    def __init__(self):
        self.config = Config()
        self.rl_path_input = str(self.config.get_rl_install_path())
        rl_path = Path(self.rl_path_input)
        self.map_manager = MapManager(rl_path)
        self.selected_map_index = -1
        self.status_message = ""
        self.status_color = (1.0, 1.0, 1.0, 1.0)
        self.window = None
        self.impl = None
        self._init_imgui()
        self._update_map_manager()

    def _init_imgui(self):
        """Initialize ImGui context."""
        imgui.create_context()
        io = imgui.get_io()
        io.display_size = (1000, 700)

    def _update_map_manager(self):
        """Update the map manager with current RL path."""
        rl_path = Path(self.rl_path_input)
        self.map_manager = MapManager(rl_path)

    def _confirm_rl_install_path(self):
        """Commit the typed Rocket League installation path."""
        self.config.set_rl_install_path(self.rl_path_input)
        self._update_map_manager()
        self._set_status("Updated Rocket League installation path", True)

    def _set_status(self, message: str, success: bool = True):
        """Set status message with color."""
        self.status_message = message
        self.status_color = (0.0, 1.0, 0.0, 1.0) if success else (1.0, 0.0, 0.0, 1.0)

    def _handle_dropped_files(self, file_paths: list):
        """Handle dropped files from drag and drop."""
        import shutil
        
        for file_path in file_paths:
            path = Path(file_path)
            if path.suffix.lower() not in [".udk", ".upk"]:
                continue
            
            try:
                target_name = path.stem + ".upk"
                target_path = self.config.maps_dir / target_name
                
                shutil.copy2(path, target_path)
               
                self.config.add_custom_map(str(target_path))
                self._set_status(f"Added map: {target_name}", True)
            except Exception as e:
                self._set_status(f"Failed to add map: {e}", False)

    def render_header(self):
        """Render header with RL install path selection."""
        io = imgui.get_io()
        imgui.set_next_window_position(0, 0)
        imgui.set_next_window_size(io.display_size[0], io.display_size[1])
        imgui.begin("RL Map Loader", flags=imgui.WINDOW_NO_MOVE)

        imgui.text("Rocket League Map Loader")
        imgui.separator()

        imgui.text("Rocket League Installation Path:")
        changed, self.rl_path_input = imgui.input_text(
            "##rl_path", self.rl_path_input, 256
        )

        if imgui.button("Confirm"):
            self._confirm_rl_install_path()

        imgui.same_line()
        if imgui.button("Reset to Default"):
            default_path = get_default_rl_path()
            self.rl_path_input = str(default_path)
            self.config.set_rl_install_path(str(default_path))
            self._update_map_manager()
            self._set_status("Reset to default path", True)

        if not self.map_manager.is_valid_rl_install():
            imgui.text_colored(
                "Invalid Rocket League installation path",
                1.0, 0.0, 0.0, 1.0
            )

        imgui.separator()

    def render_map_list(self):
        """Render custom maps list."""
        imgui.text("Custom Maps:")
        imgui.text("(Drag & drop .udk or .upk files here)")

        custom_maps = self.config.get_custom_maps()

        if len(custom_maps) == 0:
            imgui.text_colored("No custom maps added", 0.7, 0.7, 0.7, 1.0)
        else:
            imgui.text(f"Available maps: {len(custom_maps)}")
            if imgui.begin_child("maps_list", 0, 200, border=True):
                for i, map_path in enumerate(custom_maps):
                    path = Path(map_path)
                    selected = self.selected_map_index == i
                    clicked, selected = imgui.selectable(
                        path.stem, selected
                    )
                    if clicked:
                        self.selected_map_index = i

                imgui.end_child()

            if imgui.button("Remove Selected", width=200):
                if 0 <= self.selected_map_index < len(custom_maps):
                    self.config.remove_custom_map(custom_maps[self.selected_map_index])
                    self.selected_map_index = -1
                    self._set_status("Map removed", True)
                else:
                    self._set_status("No map selected", False)

        imgui.separator()

    def render_map_controls(self):
        """Render map control buttons."""
        imgui.text("Map Control:")

        custom_maps = self.config.get_custom_maps()
        has_selection = 0 <= self.selected_map_index < len(custom_maps)

        if imgui.button("Load Selected Map", width=200):
            if has_selection:
                map_path = Path(custom_maps[self.selected_map_index])
                success, message = self.map_manager.load_map(map_path)
                self._set_status(message, success)
            else:
                self._set_status("No map selected", False)

        imgui.same_line()

        restore_enabled = self.map_manager.has_custom_map_installed()
        if not restore_enabled:
            imgui.push_style_var(imgui.STYLE_ALPHA, imgui.get_style().alpha * 0.5)

        if imgui.button("Restore Original", width=200) and restore_enabled:
            success, message = self.map_manager.restore_original()
            self._set_status(message, success)
            self.selected_map_index = -1

        if not restore_enabled:
            imgui.pop_style_var()

        if self.map_manager.has_custom_map_installed():
            imgui.text_colored(
                "Custom map is currently loaded. Launch freeplay map \"Underpass - Soccar\" to load it",
                1.0, 0.8, 0.0, 1.0
            )
        else:
            imgui.text_colored(
                "Original map is active",
                0.0, 1.0, 0.0, 1.0
            )

        imgui.separator()

    def render_status(self):
        """Render status messages."""
        if self.status_message:
            imgui.text_colored(self.status_message, *self.status_color)

    def render(self):
        """Main render function."""
        imgui.new_frame()

        self.render_header()
        if self.map_manager.is_valid_rl_install():
            self.render_map_list()
            self.render_map_controls()
            self.render_status()

        imgui.end()

        imgui.render()
        data = imgui.get_draw_data()
        self.impl.render(data)

    def run(self):
        """Run the application."""
        if not glfw.init():
            print("Failed to initialize GLFW")
            return

        self.window = glfw.create_window(1000, 700, "RL Map Loader", None, None)
        if not self.window:
            print("Failed to create window")
            glfw.terminate()
            return

        glfw.make_context_current(self.window)
        self.impl = GlfwRenderer(self.window)

        glfw.set_drop_callback(self.window, lambda _, paths: self._handle_dropped_files(paths))

        while not glfw.window_should_close(self.window):
            glfw.poll_events()
            self.impl.process_inputs()
            self.render()
            glfw.swap_buffers(self.window)

        self.impl.shutdown()
        glfw.terminate()


def main():
    """Entry point for the application."""
    app = RLMapLoaderApp()
    app.run()


if __name__ == "__main__":
    main()
