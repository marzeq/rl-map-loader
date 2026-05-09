"""Configuration management for RL Map Loader."""

import json
import os
import sys
from pathlib import Path
from typing import TypedDict

from rl_map_loader.map_manager import MapManager


class JsonConfig(TypedDict):
    rl_install_path: str
    custom_maps: list[str]
    loaded_map: str | None


def get_data_dir() -> Path:
    """Get platform-specific data directory."""
    if sys.platform == "win32":
        app_data = Path(os.environ.get("APPDATA", ""))
        if not app_data.exists():
            app_data = Path.home() / "AppData" / "Roaming"
    else:
        app_data = Path.home() / ".local" / "share"

    data_dir = app_data / "rl-map-loader"
    data_dir.mkdir(parents=True, exist_ok=True)
    return data_dir


def get_default_rl_path() -> Path:
    """Get default Rocket League installation path based on OS."""
    if sys.platform == "win32":
        return Path(r"C:\Program Files\Epic Games\rocketleague")
    else:
        return Path.home() / "Games" / "Heroic" / "rocketleague"


class Config:
    """Configuration handler for RL Map Loader."""

    def __init__(self):
        self.config_file = get_data_dir() / "config.json"
        self.maps_dir = get_data_dir() / "maps"
        self.maps_dir.mkdir(parents=True, exist_ok=True)
        self.data = self._load_config()
        rl_install_path = Path(self.data.get("rl_install_path", get_default_rl_path()))
        self.map_manager = MapManager(rl_install_path)

        # migration: check if backup exists but loaded_map is none
        # if so, move backup back into original location and clear backup
        if self.map_manager.has_backup() and not self.data.get("loaded_map"):
            self.map_manager.restore_original()
            self.data["loaded_map"] = None
            self.save()

    def set_rl_install_path(self, path: Path) -> None:
        """Set the Rocket League installation path."""
        self.data["rl_install_path"] = str(path.resolve())
        self.map_manager = MapManager(path)
        self.save()

    def _load_config(self) -> JsonConfig:
        """Load configuration from file or create default."""
        if self.config_file.exists():
            with open(self.config_file, "r") as f:
                return json.load(f)
        return {
            "rl_install_path": str(get_default_rl_path()),
            "custom_maps": [],
            "loaded_map": None
        }

    def save(self) -> None:
        """Save configuration to file."""
        with open(self.config_file, "w") as f:
            json.dump(self.data, f, indent=2)

    def get_rl_install_path(self) -> Path:
        """Get the Rocket League installation path."""
        return Path(self.data.get("rl_install_path", get_default_rl_path()))

    def add_custom_map(self, map_path: str) -> None:
        """Add a custom map to the list."""
        map_path = str(Path(map_path).resolve())
        if map_path not in self.data["custom_maps"]:
            self.data["custom_maps"].append(map_path)
            self.save()

    def remove_custom_map(self, map_path: str) -> None:
        """Remove a custom map from the list and delete the file if it exists."""
        map_path_obj = Path(map_path).resolve()
        map_path_str = str(map_path_obj)
        
        if map_path_str in self.data["custom_maps"]:
            self.data["custom_maps"].remove(map_path_str)
            self.save()
            
            if map_path_obj.exists() and map_path_obj.parent == self.maps_dir:
                try:
                    map_path_obj.unlink()
                except Exception:
                    pass

        loaded_map = self.get_loaded_map()
        if loaded_map and loaded_map.resolve() == map_path_obj:
            self.restore_original()

    # proxies to map_manager

    def is_valid_rl_install(self) -> bool:
        """Check if the current RL install path is valid."""
        return self.map_manager.is_valid_rl_install()

    def get_custom_maps(self) -> list[str]:
        """Get list of custom maps."""
        return self.data.get("custom_maps", [])

    def load_map(self, map_file: Path) -> tuple[bool, str]:
        """Load a custom map and update config."""
        success, message = self.map_manager.load_map(map_file)
        if success:
            self.data["loaded_map"] = str(map_file.resolve())
            self.save()
        return success, message

    def restore_original(self) -> tuple[bool, str]:
        """Restore original map and update config."""
        success, message = self.map_manager.restore_original()
        if success:
            self.data["loaded_map"] = None
            self.save()
        return success, message

    def get_loaded_map(self) -> Path | None:
        """Get the currently loaded map file path, or None if original is loaded."""
        loaded_map = self.data.get("loaded_map")
        return Path(loaded_map) if loaded_map else None
