cmake_minimum_required(VERSION 3.10)
project(basicApi)

# FetchContent for GoogleTest and GoogleMock
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
    googlemock
    GIT_REPOSITORY https://github.com/google/googlemock.git
    GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googlemock)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Add source files
set(SOURCES
    src/Packet.cpp
    src/communication.cpp
    test/testCommunication.cpp
)

# Include directories
include_directories(${gtest_SOURCE_DIR}/include ${gmock_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sockets)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../googletest/googletest/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../googletest/googlemock/include)

# Add the external_logger subdirectory
add_subdirectory(external_logger)

# Define LOG_LEVEL for compilation (default: INFO)
set(DEFAULT_LOG_LEVEL "logger::LogLevel::INFO")
set(LOG_LEVEL ${DEFAULT_LOG_LEVEL} CACHE STRING "Set the logging level (e.g., logger::LogLevel::ERROR, logger::LogLevel::INFO, logger::LogLevel::DEBUG)")
add_definitions(-DLOG_LEVEL=${LOG_LEVEL})

# Add the test executable
add_executable(testCommunication ${SOURCES})

# Create the test executable
add_executable(LoggerTests ${TEST_SOURCES})

# Link the GTest and GMock libraries
target_link_libraries(LoggerTests
    gtest
    gmock
    pthread
)
target_link_libraries(testCommunication gtest gmock gtest_main gmock_main Threads::Threads logger)

# Enable testing
enable_testing()

# Ensure the test executable is built
add_test(NAME LoggerTests COMMAND LoggerTests)

# Add tests
add_test(NAME testCommunication COMMAND testCommunication)
