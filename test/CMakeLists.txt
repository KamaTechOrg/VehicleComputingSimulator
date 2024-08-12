cmake_minimum_required(VERSION 3.5)
project(DraggableSquaresTests)

# Find the Qt5 package
find_package(Qt5 REQUIRED COMPONENTS Test Widgets Core Gui)

# Find the BSON library
find_package(PkgConfig REQUIRED)
pkg_check_modules(BSON REQUIRED libbson-1.0)

# Set the test executable name
add_executable(DraggableSquaresTests
    test_main_window.cpp  
    ../src/draggable_square.cpp
    ../src/process.cpp
    ../src/process_dialog.cpp
    ../src/main_window.cpp
    ../src/simulation_data_manager.cpp
)


# Include BSON directories
target_include_directories(DraggableSquaresTests PRIVATE ${BSON_INCLUDE_DIRS})

# Link the test executable to the necessary Qt5 components and BSON library
target_link_libraries(DraggableSquaresTests
    Qt5::Test
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    ${BSON_LIBRARIES}
)




# Add the test
add_test(NAME DraggableSquaresTests COMMAND DraggableSquaresTests)



