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

# Добавляем наш исполняемый файл "OpenSoundDeck"
# Он будет собран из исходников, перечисленных ниже
add_executable(OpenSoundDeck
    src/main.cpp
    src/MainWindow.cpp
)

# Присоединяем (линкуем) библиотеки Qt к нашему проекту
# Это сообщает компилятору, какие модули Qt использовать
target_link_libraries(OpenSoundDeck PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
)