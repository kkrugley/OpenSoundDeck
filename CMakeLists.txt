# CMakeLists.txt

# Минимальная требуемая версия CMake
cmake_minimum_required(VERSION 3.16)

# Название проекта, язык и стандарт C++
project(OpenSoundDeck VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем автоматическую обработку файлов для Qt (MOC, RCC, UIC)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Находим библиотеку Qt6 и ее компоненты
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia Network Svg)

include_directories(third_party/miniaudio)
include_directories(src)

if (UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ALSA REQUIRED alsa)
    pkg_check_modules(LIBPULSE REQUIRED libpulse)
    find_package(X11 REQUIRED)
    # Qt6 DBus for Wayland portal support (optional - will be used if available)
    find_package(Qt6 OPTIONAL_COMPONENTS DBus)

    include_directories(${ALSA_INCLUDE_DIRS})
    include_directories(${LIBPULSE_INCLUDE_DIRS})
    include_directories(${X11_INCLUDE_DIR})
endif()

# Добавляем наш исполняемый файл "OpenSoundDeck"
# Он будет собран из исходников, перечисленных ниже
set(APP_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/SettingsDialog.cpp
    src/HotkeyCaptureDialog.cpp
    src/GlobalHotkeyManager.cpp
    src/AudioEngine.cpp
    src/VirtualAudioSetupDialog.cpp
    src/miniaudio_impl.cpp
    src/Settings.cpp
    src/SupportDialog.cpp
    src/HelpDialog.cpp
    resources.qrc
)

if(WIN32)
    add_executable(OpenSoundDeck WIN32 ${APP_SOURCES} app.rc)
else()
    add_executable(OpenSoundDeck ${APP_SOURCES})
endif()

# Присоединяем (линкуем) библиотеки Qt к нашему проекту
# Это сообщает компилятору, какие модули Qt использовать
target_link_libraries(OpenSoundDeck PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Network
    Qt6::Svg
)

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
    if(Qt6DBus_FOUND)
        target_link_libraries(OpenSoundDeck PRIVATE Qt6::DBus)
        target_compile_definitions(OpenSoundDeck PRIVATE USE_DBUS_PORTAL)
        message(STATUS "Qt6::DBus found - Wayland portal support enabled")
    else()
        message(STATUS "Qt6::DBus not found - Wayland portal support disabled")
    endif()
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
