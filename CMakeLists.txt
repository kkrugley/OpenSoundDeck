# CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

# ============================================
# Project
# ============================================
project(OpenSoundDeck VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================
# Qt6 Detection Priority:
# 1. -DCMAKE_PREFIX_PATH=... (highest priority, user explicit)
# 2. -DQt6_DIR=... (explicit Qt6 directory)
# 3. Environment variable Qt6_DIR
# 4. Environment variable QT_DIR
# 5. Auto-search in common install locations
# ============================================

# Only do auto-detection if CMAKE_PREFIX_PATH is not provided
if(NOT DEFINED CMAKE_PREFIX_PATH OR CMAKE_PREFIX_PATH STREQUAL "")
    message(STATUS "CMAKE_PREFIX_PATH not set, attempting auto-detection...")

    # Check if Qt6_DIR is already provided via command line
    if(NOT DEFINED Qt6_DIR OR Qt6_DIR STREQUAL "")
        # Try environment variables
        if(DEFINED ENV{Qt6_DIR})
            set(CMAKE_PREFIX_PATH "$ENV{Qt6_DIR}")
            message(STATUS "Using Qt6_DIR from environment: ${CMAKE_PREFIX_PATH}")
        elseif(DEFINED ENV{QT_DIR})
            set(CMAKE_PREFIX_PATH "$ENV{QT_DIR}")
            message(STATUS "Using QT_DIR from environment: ${CMAKE_PREFIX_PATH}")
        else()
            # Auto-search in common locations
            set(QT_SEARCH_PREFIXES
                "C:/Qt"
                "D:/Qt"
                "E:/Qt"
            )

            set(QT_SEARCH_VERSIONS
                "6.11.0" "6.11.1" "6.11.2"
                "6.10.0" "6.10.1" "6.10.2"
                "6.9.0" "6.9.1" "6.9.2"
                "6.8.0" "6.8.1" "6.8.2"
                "6.7.0" "6.7.1" "6.7.2" "6.7.3"
                "6.6.0" "6.6.1" "6.6.2" "6.6.3"
                "6.5.0" "6.5.1" "6.5.2" "6.5.3"
            )

            set(QT_SEARCH_COMPILERS
                "msvc2022_64"
                "msvc2019_64"
                "msvc2022"
                "msvc2019"
            )

            foreach(QT_PREFIX ${QT_SEARCH_PREFIXES})
                foreach(QT_VER ${QT_SEARCH_VERSIONS})
                    foreach(QT_COMP ${QT_SEARCH_COMPILERS})
                        set(QT_CANDIDATE "${QT_PREFIX}/${QT_VER}/${QT_COMP}")
                        if(EXISTS "${QT_CANDIDATE}/lib/cmake/Qt6/Qt6Config.cmake")
                            set(CMAKE_PREFIX_PATH "${QT_CANDIDATE}")
                            message(STATUS "Auto-detected Qt6 at: ${CMAKE_PREFIX_PATH}")
                            break()
                        endif()
                    endforeach()
                    if(DEFINED CMAKE_PREFIX_PATH AND NOT CMAKE_PREFIX_PATH STREQUAL "")
                        break()
                    endif()
                endforeach()
                if(DEFINED CMAKE_PREFIX_PATH AND NOT CMAKE_PREFIX_PATH STREQUAL "")
                    break()
                endif()
            endforeach()
        endif()
    else()
        # Qt6_DIR is set, use its parent directory as CMAKE_PREFIX_PATH
        get_filename_component(Qt6_PREFIX_PATH "${Qt6_DIR}" DIRECTORY)
        get_filename_component(Qt6_PREFIX_PATH "${Qt6_PREFIX_PATH}" DIRECTORY)
        set(CMAKE_PREFIX_PATH "${Qt6_PREFIX_PATH}")
        message(STATUS "Using Qt6_DIR: ${Qt6_DIR}")
        message(STATUS "Derived CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
    endif()
else()
    message(STATUS "Using CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
endif()

# ============================================
# Find Qt6
# ============================================
message(STATUS "Searching for Qt6...")
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia Network Svg)

if(Qt6_FOUND)
    message(STATUS "Qt6 found successfully!")
    message(STATUS "  Version: ${Qt6_VERSION}")
    message(STATUS "  Qt6_DIR: ${Qt6_DIR}")
endif()

# ============================================
# Qt Auto-generated files
# ============================================
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(third_party/miniaudio)
include_directories(src)

if (UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ALSA REQUIRED alsa)
    pkg_check_modules(LIBPULSE REQUIRED libpulse)
    find_package(X11 REQUIRED)

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

# Compiler options
if(MSVC)
    target_compile_options(OpenSoundDeck PRIVATE /W4)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:8388608")
else()
    target_compile_options(OpenSoundDeck PRIVATE -Wall -Wextra)
endif()

# ============================================
# Windows: Deploy Qt dependencies after build
# ============================================
if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt
        HINTS "${Qt6Core_DIR}/../../../bin"
        PATHS "${QT_DIR}/bin"
        ENV QT_DIR
        DOC "Path to windeployqt"
    )

    if(WINDEPLOYQT_EXECUTABLE)
        message(STATUS "Found windeployqt: ${WINDEPLOYQT_EXECUTABLE}")

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
