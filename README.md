# RL Map Loader

A Dear ImGui application for loading custom Rocket League maps on Windows and Linux.

## Features

- **Cross-platform**: Works on Windows and Linux
- **Easy setup**: On first launch, select your Rocket League installation folder
- **Drag & drop**: Add custom maps by dragging `.udk` or `.upk` files into the app
- **Safe backups**: Automatically backs up the original map before loading a custom one
- **Easy restore**: Restore the original map with a single click
- **Persistent storage**: Remembers your settings and custom maps between sessions

## Installation

### Prerequisites

- Python 3.10 or higher
- `uv` package manager

### Setup

1. Clone or navigate to this repository
2. Install dependencies:

```bash
uv sync
```

## Usage

### Running the Application

```bash
uv run main
```

Or if you've installed it as a script:

```bash
rl-map-loader
```

### First Launch

On first launch, you'll need to specify your Rocket League installation folder:

- **Windows**: Default is `C:\Program Files\Epic Games\rocketleague`
- **Linux**: Default is `~/Games/Heroic/rocketleague`

You can:
- Type or paste the path directly
- Use the "Reset to Default" button to revert to the default path

### Loading Custom Maps

1. **Add maps**: Drag `.udk` or `.upk` files directly into the app window. The app will:
   - Convert `.udk` files to `.upk` format
   - Store them in your user data directory

2. **Select a map**: Click on a map from the "Custom Maps" list

3. **Load the map**: Click the "Load Selected Map" button. The app will:
   - Back up the original `Labs_Underpass_P.upk` file (if no backup exists)
   - Replace it with your custom map

4. **Restore the original**: Click "Restore Original" to revert to the original map and clean up the backup

### Managing Custom Maps

- **Right-click** on a map in the list to remove it
- Maps are stored in:
  - **Windows**: `%APPDATA%\rl-map-loader\maps`
  - **Linux**: `~/.local/share/rl-map-loader/maps`

## How It Works

- Configuration and map paths are stored in OS-specific locations:
  - **Windows**: `%APPDATA%\rl-map-loader\config.json`
  - **Linux**: `~/.local/share/rl-map-loader/config.json`

- When you load a custom map:
  1. If no backup exists, the original map is backed up to `Labs_Underpass_P.upk.bak`
  2. Your custom map is copied to replace the original
  3. The backup remains until you restore the original

- When you restore:
  1. The backup is copied back to the original location
  2. The backup file is deleted

## Troubleshooting

- **"Invalid Rocket League installation path"**: Make sure the path points to your Rocket League install folder and contains the `CookedPCConsole` directory

- **Maps not appearing**: Make sure you're dragging actual `.udk` or `.upk` files

- **Permission denied errors**: Ensure you have write permissions to your Rocket League installation folder

## Development

### Running tests

```bash
uv run pytest
```

### Code formatting

```bash
uv run black .
uv run ruff check .
```
