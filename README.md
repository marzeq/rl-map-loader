# RL Map Loader

Application for loading custom Rocket League maps on Epic Games without Bakkesmod.

## Features

- **Cross-platform**: Works on Windows and Linux
- **Drag & drop**: Add custom maps by dragging `.udk` or `.upk` files into the app
- **Safe backups**: Automatically backs up the original map before loading a custom one
- **Easy restore**: Restore the original map with a single click

## Installation

### From releases

[<img src="./download_releases.png" width=200px>](https://github.com/marzeq/rl-map-loader/releases/latest)

Grab the appropriate release archive for your platform from the [Releases](https://github.com/marzeq/rl-map-loader/releases/latest) page,
extract it, and run the executable.

### From source

#### Prerequisites

- Python 3.10 or higher
- `uv` package manager

#### Setup

1. Clone or navigate to this repository
2. Install dependencies:

```bash
uv sync
```

#### Run

```bash
uv run rl-map-loader
```

## Usage

See the [usage guide](./USAGE.md) for detailed instructions on how to use the app.
