# 🏗️ Building OpenSoundDeck from Source

This guide will walk you through the steps to compile OpenSoundDeck on your local machine. We use CMake as our build system, which should make the process straightforward across all supported platforms.

**Important:** All active development happens in the `dev` branch.

## 1. Prerequisites

Before you begin, you'll need a few essential tools.

*   **Git:** To clone the repository.
*   **C++ Compiler:** A modern compiler that supports C++20.
    *   Linux: GCC or Clang
    *   Windows: MSVC (Visual Studio)
    *   macOS: Clang (Xcode)
*   **CMake:** Version 3.16 or higher.
*   **Qt 6:** Version 6.2 or higher. The easiest way to get it is via the [official online installer](https://www.qt.io/download-qt-installer).
    *   When installing Qt, make sure to select the following components:
        *   `Qt Core` (includes the Qt Test module)
        *   `Qt GUI`
        *   `Qt Widgets`
        *   `Qt Linguist Tools` (for internationalization)

---

## 2. Platform-Specific Setup

### 🐧 Linux (Debian/Ubuntu Example)

1.  **Install build tools and Git:**
    ```bash
    sudo apt update
    sudo apt install build-essential git cmake
    ```

2.  **Install Qt 6 development libraries:**
    ```bash
    sudo apt install qt6-base-dev qt6-l10n-tools
    ```
    *(Note: The package name may vary on other distributions).*

### 🪟 Windows

1.  **Install Visual Studio 2019 or later:**
    *   Download [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/).
    *   During installation, select the **"Desktop development with C++"** workload.

2.  **Install Qt 6:**
    *   Run the Qt online installer.
    *   In the "Select Components" screen, choose the Qt version that matches your compiler (e.g., `MSVC 2019 64-bit`) and ensure the modules listed in the prerequisites are selected.

### 🍎 macOS

1.  **Install Xcode Command Line Tools:**
    ```bash
    xcode-select --install
    ```

2.  **Install dependencies with Homebrew:**
    *   If you don't have Homebrew, [install it first](https://brew.sh/).
    *   Then, install CMake and Qt 6:
    ```bash
    brew install cmake qt@6
    ```

---

## 3. Cloning and Building

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/kkrugley/OpenSoundDeck.git
    cd OpenSoundDeck
    ```

2.  **Switch to the `dev` branch:** This is where the latest code lives.
    ```bash
    git checkout dev
    ```

3.  **Configure the project with CMake:**
    *   We recommend a separate build directory.
    ```bash
    mkdir build
    cd build
    ```
    *   Run CMake. We enable tests by default with `BUILD_TESTING=ON`.
    ```bash
    # On Linux and macOS:
    cmake .. -DBUILD_TESTING=ON

    # On Windows (if you need to specify the generator):
    cmake .. -G "Visual Studio 17 2022" -DBUILD_TESTING=ON
    ```
    *Tip: If CMake cannot find Qt, you may need to provide a hint:*
    `cmake .. -DCMAKE_PREFIX_PATH=/path/to/your/Qt/6.x.x/msvc2019_64`

4.  **Compile the project and tests:**
    ```bash
    cmake --build .
    ```
    *   Alternatively, on Linux/macOS, you can run `make`, or on Windows, open the `.sln` file in Visual Studio and build the "ALL_BUILD" project.

---

## 4. Running Tests

Quality is critical to this project. After a successful build, you should run the test suite to ensure everything is working correctly.

From your `build` directory, run:
```bash
ctest
```
All tests should pass before you consider submitting code.

## 5. Running the Application

The executable will be located inside the `build` directory (or a subdirectory like `build/src/` depending on the project structure).