"""Map management and backup logic for RL Map Loader."""

import shutil
from pathlib import Path


class MapManager:
    """Handles map file operations, backups, and restoration."""

    TARGET_MAP = "Labs_Underpass_P.upk"
    BACKUP_EXT = ".upk.bak"

    def __init__(self, rl_install_path: Path):
        self.rl_install_path = Path(rl_install_path)
        self.cooked_dir = self.rl_install_path / "TAGame" / "CookedPCConsole"
        self.target_file = self.cooked_dir / self.TARGET_MAP
        self.backup_file = self.cooked_dir / f"{self.TARGET_MAP}{self.BACKUP_EXT}"

    def is_valid_rl_install(self) -> bool:
        """Check if the path is a valid Rocket League installation."""
        return self.cooked_dir.exists() and self.cooked_dir.is_dir()

    def has_backup(self) -> bool:
        """Check if a backup of the original map exists."""
        return self.backup_file.exists()

    def has_custom_map_installed(self) -> bool:
        """Check if a custom map is currently installed (backup exists)."""
        return self.has_backup()

    def load_map(self, map_file: Path) -> tuple[bool, str]:
        """
        Load a custom map.

        If a backup doesn't exist, create one first.
        If a backup exists, don't create another one.

        Returns (success, message)
        """
        map_file = Path(map_file)

        if not map_file.exists():
            return False, f"Map file not found: {map_file}"

        if map_file.suffix.lower() != ".upk":
            return False, f"Invalid map file extension: {map_file.suffix}"

        if not self.cooked_dir.exists():
            self.cooked_dir.mkdir(parents=True, exist_ok=True)

        if not self.has_backup() and self.target_file.exists():
            try:
                shutil.copy2(self.target_file, self.backup_file)
            except Exception as e:
                return False, f"Failed to create backup: {e}"

        try:
            shutil.copy2(map_file, self.target_file)
            return True, f"Successfully loaded map: {map_file.name}"
        except Exception as e:
            return False, f"Failed to load map: {e}"

    def restore_original(self) -> tuple[bool, str]:
        """
        Restore the original map from backup.

        Removes the backup file after restoration.

        Returns (success, message)
        """
        if not self.has_backup():
            return False, "No backup found"

        try:
            shutil.copy2(self.backup_file, self.target_file)
            self.backup_file.unlink()
            return True, "Successfully restored original map"
        except Exception as e:
            return False, f"Failed to restore map: {e}"

    def normalize_map_file(self, map_file: Path) -> Path:
        """Convert .udk files to .upk extension."""
        map_file = Path(map_file)
        if map_file.suffix.lower() == ".udk":
            upk_file = map_file.with_suffix(".upk")
            try:
                shutil.copy2(map_file, upk_file)
                return upk_file
            except Exception:
                return map_file
        return map_file
