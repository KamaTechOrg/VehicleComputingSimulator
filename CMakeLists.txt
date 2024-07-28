cmake_minimum_required(VERSION 3.5)

project(ProcessManagerApp)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(ProcessManagerApp
    main.cpp
    MainWindow.cpp
    MainWindow.h
)

target_link_libraries(ProcessManagerApp Qt5::Core Qt5::Gui Qt5::Widgets)
