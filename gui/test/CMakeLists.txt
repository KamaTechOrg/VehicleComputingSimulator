cmake_minimum_required(VERSION 3.5)
project(DraggableSquaresTests)
# Find the BSON library
find_package(PkgConfig REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Test Sql Gui)

pkg_check_modules(BSON REQUIRED libbson-1.0)
# Define the list of source files
set(src_files
    ../src/draggable_square.cpp
    ../src/process.cpp
    ../src/process_dialog.cpp
    ../src/main_window.cpp
    ../src/simulation_data_manager.cpp
    ../src/frames.cpp
    ../src/log_handler.cpp
    ../src/dataToSql.cpp
)
# Set the test executable name for MainWindowTests
add_executable(MainWindowTests
    test_main_window.cpp
    ${src_files}
)
# Set the test executable name for DraggableSquareTests
add_executable(DraggableSquareTests
    test_draggable_square.cpp
    ${src_files}
)
# Set the test executable name for UserInteractionTests
add_executable(UserInteractionTests
    user_interaction_tests.cpp
    ${src_files}
)

add_executable(ProcessTests
    test_process.cpp
    ${src_files}
)

add_executable(ProcessDialogTests
    test_process_dialog.cpp
    ${src_files}
)
add_executable(FrameTests
    test_frames.cpp
     ${src_files}
)
add_executable(DataToSql
test_dataToSql.cpp     
${src_files}
)
# Include BSON directories for both targets
target_include_directories(MainWindowTests PRIVATE ${BSON_INCLUDE_DIRS})
target_include_directories(DraggableSquareTests PRIVATE ${BSON_INCLUDE_DIRS})
target_include_directories(UserInteractionTests PRIVATE ${BSON_INCLUDE_DIRS})
target_include_directories(ProcessTests PRIVATE ${BSON_INCLUDE_DIRS})
target_include_directories(ProcessDialogTests PRIVATE ${BSON_INCLUDE_DIRS})
target_include_directories(FrameTests PRIVATE ${BSON_INCLUDE_DIRS})
target_include_directories(DataToSql PRIVATE ${BSON_INCLUDE_DIRS})


# Link the test executables to the necessary Qt5 components and BSON library
target_link_libraries(MainWindowTests
    Qt5::Test
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::Sql
    ${BSON_LIBRARIES}
)
target_link_libraries(DraggableSquareTests
    Qt5::Test
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::Sql
    ${BSON_LIBRARIES}
)
target_link_libraries(UserInteractionTests
    Qt5::Test
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::Sql
    ${BSON_LIBRARIES}
)
target_link_libraries(ProcessTests
    Qt5::Test
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::Sql
    ${BSON_LIBRARIES}
)
target_link_libraries(ProcessDialogTests
    Qt5::Test
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::Sql
    ${BSON_LIBRARIES}
)
target_link_libraries(FrameTests
    Qt5::Test
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::Sql

    ${BSON_LIBRARIES}
)
target_link_libraries(DataToSql
    Qt5::Test
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::Sql

    ${BSON_LIBRARIES}
)
set(CMAKE_PREFIX_PATH "/usr/local" ${CMAKE_PREFIX_PATH})
# Include directories
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
# Link against Google Test and Google Mock
target_link_libraries(MainWindowTests ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} pthread)
target_link_libraries(DraggableSquareTests ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} pthread)
target_link_libraries(UserInteractionTests ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} pthread)
target_link_libraries(ProcessTests ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} pthread)
target_link_libraries(ProcessDialogTests ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} pthread)
target_link_libraries(FrameTests ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} pthread)
target_link_libraries(DataToSql ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} pthread)

add_test(NAME MainWindowTests COMMAND MainWindowTests)
add_test(NAME DraggableSquareTests COMMAND DraggableSquareTests)
add_test(NAME UserInteractionTests COMMAND UserInteractionTests)
add_test(NAME ProcessTests COMMAND ProcessTests)
add_test(NAME ProcessDialogTests COMMAND ProcessDialogTests)
add_test(NAME FrameTests COMMAND FrameTests)
add_test(NAME DataToSql COMMAND DataToSql)






