# 🏗️ Building OpenSoundDeck from Source

This guide covers building OpenSoundDeck on all supported platforms. The project uses **CMake** and **Qt6**.

**⚠️ Important:** Binary releases are not yet available. You must build OpenSoundDeck from source to use it.

**⚠️ Development:** Active development is on the `dev` branch. `main` contains release-only code. Always build from `dev` for development.

---

## 1. Prerequisites

| Tool | Version | Purpose |
|------|---------|---------|
| **Git** | Any | Clone repository |
| **CMake** | 3.16+ | Build system |
| **C++ Compiler** | C++20 support | GCC 10+, Clang 12+, MSVC 2019+ |
| **Qt 6** | 6.2+ | GUI framework |

### Required Qt 6 Components

Install these via the [Qt Online Installer](https://www.qt.io/download-qt-installer):
- ✅ **Qt Core** (includes Qt Test)
- ✅ **Qt GUI**
- ✅ **Qt Widgets**
- ✅ **Qt Multimedia**
- ✅ **Qt Network** (for driver downloads)
- ✅ **Qt Linguist Tools** (for i18n)

---

## 2. Platform-Specific Setup

### 🐧 Linux (Debian/Ubuntu)

```bash
# Build tools and Qt6
sudo apt update
sudo apt install build-essential git cmake
sudo apt install qt6-base-dev qt6-multimedia-dev libqt6network6

# Audio libraries (ALSA + PulseAudio)
sudo apt install libasound2-dev libpulse-dev libx11-dev

# Optional: PipeWire support
sudo apt install libpipewire-0.3-dev
```

**Note:** No virtual audio driver needed. OpenSoundDeck creates virtual devices automatically via PulseAudio/PipeWire.

### 🪟 Windows

#### 2.1 Install Visual Studio 2019 or later
1. Download [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/)
2. During install, select **"Desktop development with C++"** workload
3. Ensure "C++ CMake tools for Windows" is selected

#### 2.2 Install Qt 6
1. Run the Qt online installer
2. Select Qt version matching your compiler (e.g., `MSVC 2019 64-bit`)
3. Include components: Core, GUI, Widgets, Multimedia, Network, Linguist Tools

#### 2.3 Install CMake
```powershell
# Via winget
winget install Kitware.CMake

# Or download from: https://cmake.org/download/
```

#### 2.4 Virtual Audio Driver (Required for Mic Injection)
Before running OpenSoundDeck, you **must** install VB-Audio Virtual Cable:

1. Download from [vb-audio.com](https://vb-audio.com/Cable/)
2. Run `VBCABLE_Setup.exe` or `VBCABLE_Setup_x64.exe`
3. Reboot Windows

*Note: First run of OpenSoundDeck will guide you through this setup.*

### 🍎 macOS

#### 2.1 Xcode Command Line Tools
```bash
xcode-select --install
```

#### 2.2 Homebrew dependencies
```bash
brew install cmake qt@6
```

#### 2.3 Virtual Audio Driver (Required for Mic Injection)
Install BlackHole (open-source virtual audio driver):

```bash
brew install blackhole-2ch
```

*Or download manually from [GitHub](https://github.com/ExistentialAudio/BlackHole)*

After installation:
1. Open **System Preferences → Security & Privacy**
2. Click **"Allow"** for the BlackHole driver
3. Reboot macOS

---

## 3. Cloning and Building

```bash
# Clone repository
git clone https://github.com/kkrugley/OpenSoundDeck.git
cd OpenSoundDeck

# Switch to dev branch (important!)
git checkout dev

# Create build directory
mkdir build
cd build

# Configure with CMake
# CMake автоматически ищет Qt6 в типичных местах установки.
# Если Qt6 не найден, укажите путь вручную:
#
# Windows: -DCMAKE_PREFIX_PATH=C:/Qt/6.x.x/msvc2022_64
# macOS:   -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6
# Linux:   -DCMAKE_PREFIX_PATH=/usr/lib/qt6
#
# Или через переменную окружения:
# Windows PowerShell: $env:Qt6_DIR="C:/Qt/6.8.0/msvc2022_64/lib/cmake/Qt6"
# Windows CMD:        set Qt6_DIR=C:\Qt\6.8.0\msvc2022_64\lib\cmake\Qt6

cmake ..

# Build
cmake --build . --config Release
```

### Platform-Specific Build Commands

**Windows (Visual Studio):**
```powershell
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

**Linux/macOS (Make):**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)  # or just: cmake --build .
```

---

## 4. Running the Application

### Windows
```
build\Release\OpenSoundDeck.exe
```

### Linux
```
./build/OpenSoundDeck
```

### macOS
```
./build/OpenSoundDeck.app/Contents/MacOS/OpenSoundDeck
# Or open the bundle:
open ./build/OpenSoundDeck.app
```

**First Run:**
On Windows/macOS, you'll see a "Virtual Audio Setup" dialog if the virtual audio driver is not detected. Follow the instructions to install and configure VB-Cable (Win) or BlackHole (Mac).

---

## 5. Troubleshooting

### CMake cannot find Qt
```bash
# Add Qt to PATH or specify explicitly:
cmake .. -DCMAKE_PREFIX_PATH=C:\Qt\6.8.0\msvc2022_64
```

### "Failed to initialize audio engine" on Windows
- Ensure VB-Audio Virtual Cable is installed
- Check Windows Sound Settings → Recording devices
- Look for "CABLE Output" device

### "Failed to initialize audio engine" on macOS
- BlackHole driver must be approved in System Preferences
- Reboot after installing BlackHole

### Linux: No sound in virtual microphone
- Check PulseAudio is running: `pulseaudio --check` or `systemctl --user status pipewire`
- Verify virtual sink created: `pactl list sinks | grep virt_mic`
- Restart OpenSoundDeck

### Windows: Link errors (unresolved externals)
Ensure Windows SDK is installed with Visual Studio:
- Visual Studio Installer → Modify → "Desktop development with C++" → "Windows SDK"

### Windows: "Virtual Audio Setup" dialog crashes or hangs
This is typically a COM initialization issue. The fix involves:
1. Ensure Windows SDK includes `propvarutil.h` (for PropVariantInit/PropVariantClear)
2. COM must use `COINIT_APARTMENTTHREADED` to be compatible with Qt
3. Proper resource cleanup - only call `CoUninitialize()` if `CoInitializeEx()` succeeded

If you see crashes in `checkVBCableInstalled()` or `configureWindowsAudio()`, check:
- Windows SDK version compatibility
- COM apartment model matches Qt's requirement
- No memory leaks in PROPVARIANT handling

### Windows: 36-second delay then crash
This was caused by improper COM initialization in earlier versions. Fixed by:
- Using `COINIT_APARTMENTTHREADED` instead of `COINIT_MULTITHREADED`
- Checking `RPC_E_CHANGED_MODE` to avoid double uninitialization
- Adding null checks before releasing COM interfaces

---

## 6. Development Workflow

**Before submitting PR:**
1. Build from `dev` branch
2. Test on your platform
3. Check code compiles without warnings
4. Ensure global hotkeys work (test with app not in focus)

**Platform testing priority:**
- Windows: Most common for gaming/streaming
- Linux: PulseAudio/PipeWire variations
- macOS: BlackHole + CoreAudio quirks

---

## 7. IDE Setup

### Visual Studio (Windows)
1. Open folder in VS: `File → Open → CMake...`
2. Select `CMakeLists.txt`
3. VS will auto-detect configuration

### Qt Creator
1. Open `CMakeLists.txt`
2. Kit selection: Choose your Qt6 kit
3. Build & Run

### VS Code
Install extensions:
- CMake Tools
- C/C++
- Qt tools (optional)

Configure in `.vscode/settings.json`:
```json
{
  "cmake.configureSettings": {
    "CMAKE_PREFIX_PATH": "/path/to/Qt/6.x.x/gcc_64"
  }
}
```

---

**Need help?** Open an issue with:
- Platform and version
- CMake output (with errors)
- Qt version
- Compiler version
