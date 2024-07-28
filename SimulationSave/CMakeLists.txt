cmake_minimum_required(VERSION 3.10)
project(SimulationSave)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBBSON REQUIRED libbson-1.0)

# Enable Qt MOC processing
set(CMAKE_AUTOMOC ON)

add_executable(SimulationSave main.cpp SimulationDataManager.cpp)

target_include_directories(SimulationSave PRIVATE ${LIBBSON_INCLUDE_DIRS})
target_link_libraries(SimulationSave Qt5::Widgets ${LIBBSON_LIBRARIES})
