# Building RL Map Loader

## Quick Start

```bash
git submodule init && git submodule update --recursive
meson setup build
ninja -C build
./build/rl-map-loader
```

## Requirements

### Ubuntu/Debian
```bash
sudo apt-get install build-essential meson ninja-build pkg-config libgl-dev libglfw3-dev cmake
```

### Fedora/RHEL
```bash
sudo yum install gcc-c++ meson ninja-build pkg-config mesa-libGL-devel glfw-devel cmake
```

### Arch

```bash
sudo pacman -S base-devel meson ninja pkgconf mesa libglvnd glfw cmake
```

### Windows

**Using MSVC with Visual Studio:**
1. Install [Visual Studio Community](https://visualstudio.microsoft.com/community/) with C++ workload
2. Install [Meson](https://meson-build.com/) and [Ninja](https://ninja-build.org/)
3. Install [vcpkg](https://github.com/Microsoft/vcpkg):
   ```cmd
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\vcpkg.exe integrate install
   .\vcpkg.exe install glfw3:x64-windows opengl:x64-windows
   ```
4. Configure and build:
   ```cmd
   git submodule init && git submodule update --recursive
   meson setup build --prefix=%VCPKG_ROOT%
   ninja -C build
   build\rl-map-loader.exe
   ```

**Using MinGW:**
1. Install [MSYS2](https://www.msys2.org/)
2. In MSYS2 terminal:
   ```bash
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-meson mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-pkgconf mingw-w64-ucrt-x86_64-mesa
   ```
3. Configure and build:
   ```bash
   git submodule init && git submodule update --recursive
   meson setup build
   ninja -C build
   ./build/rl-map-loader.exe
   ```

## Build Variants

**Release (default):**
```bash
meson setup build
ninja -C build
```

**Debug:**
```bash
meson setup build -Dbuildtype=debug
ninja -C build
```

**Minimal size:**
```bash
meson setup build -Dbuildtype=minsize
ninja -C build
```

## Common Tasks

**Linux:**
```bash
# Reconfigure existing build
meson configure build -Dbuildtype=debug

# Rebuild from scratch
rm -rf build && meson setup build && ninja -C build

# Use multiple cores
ninja -C build -j$(nproc)

# Install system-wide
ninja -C build install

# Verbose output
ninja -C build -v
```

**Windows (MinGW/MSYS2):**
```bash
# Reconfigure existing build
meson configure build -Dbuildtype=debug

# Rebuild from scratch
rm -rf build && meson setup build && ninja -C build

# Use multiple cores
ninja -C build -j%NUMBER_OF_PROCESSORS%

# Verbose output
ninja -C build -v
```

**Windows (Command Prompt):**
```cmd
REM Rebuild from scratch
rmdir /s /q build
meson setup build
ninja -C build
```

## Troubleshooting

**"glfw3 not found"**
- Ubuntu: `sudo apt-get install libglfw3-dev`
- Fedora: `sudo yum install glfw-devel`
- Arch: `sudo pacman -S glfw`
- Windows (MSVC): Ensure vcpkg packages are installed and `--prefix` is set correctly
- Windows (MinGW): `pacman -S mingw-w64-ucrt-x86_64-glfw`

**"GL not found"**
- Ubuntu: `sudo apt-get install libgl-dev`
- Fedora: `sudo yum install mesa-libGL-devel`
- Arch: `sudo pacman -S mesa libglvnd`
- Windows: Should be included with MSVC or MinGW

**Build is slow**
- Linux: Use `ninja -C build -j$(nproc)` to use all CPU cores
- Windows (MinGW): Use `ninja -C build -j%NUMBER_OF_PROCESSORS%`
