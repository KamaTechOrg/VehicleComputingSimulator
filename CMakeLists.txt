cmake_minimum_required(VERSION 3.5)

project(QtCMakeExample)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(QtCMakeExample main.cpp)

target_link_libraries(QtCMakeExample Qt5::Widgets)
