# CMakeLists.txt for /VehicleComputingSimulator/communication/user1
# Specify the minimum CMake version required
cmake_minimum_required(VERSION 3.10)
# Set the project name
project(User2)
# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# Add the path to the source files
set(SOURCES
    ../communication/src/communication.cpp
    ../communication/src/client_connection.cpp
    ../communication/src/packet.cpp
    ../communication/src/message.cpp
    ../logger/logger.cpp
    ../communication/sockets/real_socket.h
    # Add additional source files if needed
)
# Add the executable for user_2
add_executable(user_2 user_2.cpp ${SOURCES})
# Include directories for header files
include_directories(
    ../communication/src
    ../communication/sockets
    # Add more directories if needed
)
# Link the executable with the necessary libraries
target_link_libraries(user_2
    pthread
    # Add more libraries if needed
)
# Copy config.json to the build directory
configure_file(${CMAKE_SOURCE_DIR}/config.json ${CMAKE_BINARY_DIR}/config.json COPYONLY)
# Optionally, add custom commands or additional configuration here