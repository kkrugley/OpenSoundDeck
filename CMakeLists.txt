# CMakeLists.txt

# Минимальная требуемая версия CMake
cmake_minimum_required(VERSION 3.16)

# Название проекта, язык и стандарт C++
project(OpenSoundDeck VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================
# Qt6 Detection
# ============================================
# Priority (highest to lowest):
# 1. Qt6_DIR CMake variable (set via -DQt6_DIR=... or env var)
# 2. CMAKE_PREFIX_PATH (set via -DCMAKE_PREFIX_PATH=...)
# 3. QT_DIR environment variable
# 4. Qt6_DIR environment variable
# 5. Automatic search in common install locations

# Check environment variables first (lowest priority, so set early)
if(NOT Qt6_DIR AND DEFINED ENV{Qt6_DIR})
    set(Qt6_DIR "$ENV{Qt6_DIR}")
    message(STATUS "Using Qt6_DIR from environment variable: ${Qt6_DIR}")
endif()

if(NOT Qt6_DIR AND DEFINED ENV{QT_DIR})
    set(Qt6_DIR "$ENV{QT_DIR}/lib/cmake/Qt6")
    message(STATUS "Using QT_DIR from environment variable: ${Qt6_DIR}")
endif()

# If still not found, try automatic detection in common locations
if(NOT Qt6_DIR)
    # Common Qt installation paths on Windows
    set(QT_SEARCH_PATHS
        "C:/Qt/6.8.0/msvc2022_64/lib/cmake/Qt6"
        "C:/Qt/6.8.0/msvc2019_64/lib/cmake/Qt6"
        "C:/Qt/6.7.0/msvc2022_64/lib/cmake/Qt6"
        "C:/Qt/6.7.0/msvc2019_64/lib/cmake/Qt6"
        "C:/Qt/6.6.0/msvc2022_64/lib/cmake/Qt6"
        "C:/Qt/6.6.0/msvc2019_64/lib/cmake/Qt6"
        "C:/Qt/6.5.0/msvc2022_64/lib/cmake/Qt6"
        "C:/Qt/6.5.0/msvc2019_64/lib/cmake/Qt6"
        "D:/Qt/6.8.0/msvc2022_64/lib/cmake/Qt6"
        "D:/Qt/6.8.0/msvc2019_64/lib/cmake/Qt6"
        "D:/Qt/6.7.0/msvc2022_64/lib/cmake/Qt6"
        "D:/Qt/6.7.0/msvc2019_64/lib/cmake/Qt6"
    )

    foreach(QT_PATH ${QT_SEARCH_PATHS})
        if(EXISTS "${QT_PATH}/Qt6Config.cmake")
            set(Qt6_DIR "${QT_PATH}")
            message(STATUS "Found Qt6 at: ${Qt6_DIR}")
            break()
        endif()
    endforeach()
endif()

# ============================================
# Find Qt6
# ============================================
# Qt6_DIR can be:
# - Set via command line: -DQt6_DIR=C:/Qt/6.x.x/msvc2022_64/lib/cmake/Qt6
# - Set via CMAKE_PREFIX_PATH: -DCMAKE_PREFIX_PATH=C:/Qt/6.x.x/msvc2022_64
# - Set from environment variables (see above)
# - Found automatically in common locations

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia Network Svg)

if(Qt6_FOUND)
    message(STATUS "Qt6 found: ${Qt6_VERSION}")
    message(STATUS "Qt6 directory: ${Qt6_DIR}")
endif()

# ============================================
# Project Configuration
# ============================================

# Включаем автоматическую обработку файлов для Qt (MOC, RCC, UIC)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Для сетевых операций (загрузка драйверов)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(third_party/miniaudio)
include_directories(src)

if (UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ALSA REQUIRED alsa)
    pkg_check_modules(LIBPULSE REQUIRED libpulse)
    find_package(X11 REQUIRED)

    # Добавляем пути к заголовочным файлам системных библиотек
    include_directories(${ALSA_INCLUDE_DIRS})
    include_directories(${LIBPULSE_INCLUDE_DIRS})
    include_directories(${X11_INCLUDE_DIR})
endif()

# ============================================
# Source Files
# ============================================
set(APP_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/SettingsDialog.cpp
    src/HotkeyCaptureDialog.cpp
    src/GlobalHotkeyManager.cpp
    src/AudioEngine.cpp
    src/VirtualAudioSetupDialog.cpp
    src/miniaudio_impl.cpp
    resources.qrc
)

# ============================================
# Executable
# ============================================
if(WIN32)
    add_executable(OpenSoundDeck WIN32 ${APP_SOURCES})
elseif(APPLE)
    add_executable(OpenSoundDeck ${APP_SOURCES})
else()
    add_executable(OpenSoundDeck ${APP_SOURCES})
endif()

# ============================================
# Link Libraries
# ============================================
target_link_libraries(OpenSoundDeck PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Network
    Qt6::Svg
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(OpenSoundDeck PRIVATE
        winmm
        ole32
        oleaut32
        ksuser
    )
endif()

if (UNIX AND NOT APPLE)
    target_link_libraries(OpenSoundDeck PRIVATE
        ${ALSA_LIBRARIES}
        ${LIBPULSE_LIBRARIES}
        ${X11_LIBRARIES}
        pthread
    )
endif()

if(APPLE)
    # macOS frameworks
    find_library(COREAUDIO_LIBRARY CoreAudio)
    find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox)
    find_library(CARBON_LIBRARY Carbon)

    target_link_libraries(OpenSoundDeck PRIVATE
        ${COREAUDIO_LIBRARY}
        ${AUDIOTOOLBOX_LIBRARY}
        ${CARBON_LIBRARY}
    )

    set_target_properties(OpenSoundDeck PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_ICON_FILE icons/app.icns
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
    )
endif()

# Compiler-specific options for better audio performance
if(MSVC)
    target_compile_options(OpenSoundDeck PRIVATE /W4)
    # Increase stack size for miniaudio (default is 1MB, increase to 8MB)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:8388608")
else()
    target_compile_options(OpenSoundDeck PRIVATE -Wall -Wextra)
endif()

# ============================================
# Windows: Deploy Qt dependencies after build
# ============================================
if(WIN32)
    # Find windeployqt
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt
        HINTS "${Qt6Core_DIR}/../../../bin"
        PATHS "${QT_DIR}/bin"
        ENV QT_DIR
        DOC "Path to windeployqt"
    )

    if(WINDEPLOYQT_EXECUTABLE)
        message(STATUS "Found windeployqt: ${WINDEPLOYQT_EXECUTABLE}")

        # Add post-build command to copy Qt dependencies
        add_custom_command(TARGET OpenSoundDeck POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            --release
            --no-compiler-runtime
            --no-translations
            "$<TARGET_FILE:OpenSoundDeck>"
            COMMENT "Deploying Qt dependencies..."
        )
    else()
        message(WARNING "windeployqt not found! You need to run it manually or add Qt to PATH.")
    endif()
endif()
