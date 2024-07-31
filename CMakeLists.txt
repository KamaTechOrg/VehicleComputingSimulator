cmake_minimum_required(VERSION 3.10)

# Project name
# <<<<<<< HEAD
# project(CommunicationProject)

# # Set C++ standard
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

# # Add source files
# set(SOURCES
#     communication.cpp
#     myClass.cpp
#     testCommunication
# )

# # Add the executable target
# add_executable(communication_test ${SOURCES})

# # Add the directory where doctest.h is located
# target_include_directories(communication_test PRIVATE ${CMAKE_SOURCE_DIR})

# # Link libraries (if any)
# # For example, if you need to link against pthread for threading
# # target_link_libraries(communication_test pthread)
# =======
project(BasicApi)

# Set standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Add source files
set(COMMON_SOURCES
    communication.cpp
    myClass.cpp
    Packet.cpp
)

# Create executable for user1
add_executable(user1 user1.cpp ${COMMON_SOURCES})

# Create executable for user2
add_executable(user2 user2.cpp ${COMMON_SOURCES})

# Link pthread library
target_link_libraries(user1 pthread)
target_link_libraries(user2 pthread)
