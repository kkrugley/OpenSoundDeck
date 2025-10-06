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
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia)

include_directories(third_party/miniaudio)

if (UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ALSA REQUIRED alsa)
    pkg_check_modules(LIBPULSE REQUIRED libpulse)
    
    # Добавляем пути к заголовочным файлам системных библиотек
    include_directories(${ALSA_INCLUDE_DIRS})
    include_directories(${LIBPULSE_INCLUDE_DIRS})
endif()

# Добавляем наш исполняемый файл "OpenSoundDeck"
# Он будет собран из исходников, перечисленных ниже
set(APP_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/AudioEngine.cpp
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
)

if (UNIX AND NOT APPLE)
    target_link_libraries(OpenSoundDeck PRIVATE
        ${ALSA_LIBRARIES}
        ${LIBPULSE_LIBRARIES}
        pthread
    )
endif()

if(APPLE)
    set_target_properties(OpenSoundDeck PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_ICON_FILE icons/app.icns
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
    )
endif()