cmake_minimum_required(VERSION 3.10)
project(VehicleComputingSimulator)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add GoogleTest and GoogleMock
add_subdirectory(googletest)

# Include directories
include_directories(${PROJECT_SOURCE_DIR})

# Source files
set(SOURCES
    src/client.cpp
    src/communication.cpp
    logger/logger.cpp
    src/message.cpp
    src/communication.cpp
)

# Executable
add_executable(VehicleComputingSimulator ${SOURCES})

# Link libraries
target_link_libraries(VehicleComputingSimulator gtest gtest_main gmock gmock_main)

# Add test
enable_testing()
add_executable(client_test
    src/client.cpp
    tests/client_test.cpp
    logger/logger.cpp
    src/message.cpp
    src/communication.cpp
)
add_executable(server_test
    src/server.cpp
    tests/server_test.cpp
    logger/logger.cpp
    src/message.cpp
    src/manager.cpp
)
# Link test executable with gtest and gmock
target_link_libraries(client_test gtest gtest_main gmock gmock_main)
target_link_libraries(server_test gtest gtest_main gmock gmock_main)

# Add tests
add_test(NAME client_test COMMAND client_test)
add_test(NAME server_test COMMAND server_test)