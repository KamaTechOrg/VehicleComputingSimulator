cmake_minimum_required(VERSION 3.10)

# Project name
project(CommunicationProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files
set(SOURCES
    communication.cpp
    myClass.cpp
    testCommunication
)

# Add the executable target
add_executable(communication_test ${SOURCES})

# Add the directory where doctest.h is located
target_include_directories(communication_test PRIVATE ${CMAKE_SOURCE_DIR})

# Link libraries (if any)
# For example, if you need to link against pthread for threading
# target_link_libraries(communication_test pthread)
