#!/usr/bin/env python
"""Build script for RL Map Loader using PyInstaller."""

import subprocess
import sys
from pathlib import Path


def build():
    """Build the application into a standalone executable.
    
    Args:
        target_os: Target OS ('linux', 'windows', 'macos') or None for current OS.
                   Note: PyInstaller cannot cross-compile; specify target for documentation.
    """
    root = Path(__file__).parent
    current_os = sys.platform
    
    args = [
        "pyinstaller",
        "--onefile",
        "--windowed",
        "--name=rl-map-loader",
        "--hidden-import=rl_map_loader",
        "--hidden-import=rl_map_loader.main",
        "--hidden-import=rl_map_loader.config",
        "--hidden-import=rl_map_loader.map_manager",
        "--hidden-import=imgui",
        "--hidden-import=glfw",
        "--hidden-import=OpenGL",
        "--hidden-import=OpenGL.platform",
        "--hidden-import=OpenGL.platform.glx",
        "--hidden-import=OpenGL.platform.egl",
        "--hidden-import=OpenGL.platform.x11",
        "--collect-all=glfw",
        "--collect-all=imgui",
        "--collect-all=OpenGL",
        "--add-data=src/rl_map_loader:rl_map_loader",
        "run.py",
    ]
    
    print(f"Building for {current_os}...")
    print(f"Command: {' '.join(args)}\n")
    result = subprocess.run(args, cwd=root)
    
    if result.returncode == 0:
        if sys.platform == "win32":
            exe_path = root / "dist" / "rl-map-loader.exe"
            print(f"\nBuild successful!")
            print(f"Executable: {exe_path}")
        else:
            exe_path = root / "dist" / "rl-map-loader"
            print(f"\nBuild successful!")
            print(f"Executable: {exe_path}")
        print(f"\nRun with: {exe_path}")
    else:
        print(f"\nBuild failed with exit code {result.returncode}")
        sys.exit(1)


if __name__ == "__main__":
    build()
