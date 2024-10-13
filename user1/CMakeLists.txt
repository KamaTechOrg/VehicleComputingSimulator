cmake_minimum_required(VERSION 3.10)
project("user_1" LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)



# # Path to local communication directory
# set(HSM_CLIENT_PATH "${CMAKE_SOURCE_DIR}/../hsm-client")

# # Add communication
# add_subdirectory(${HSM_CLIENT_PATH} ${CMAKE_BINARY_DIR}/hsm-client)

file(GLOB SOURCES_COMMUNICATION "../communication/src/*.cpp")
file(GLOB HEADERS_COMMUNICATION "../communication/include/*.h")
file(GLOB SOCKETS_COMMUNICATIONS "../communication/sockets/*.*" )

file(GLOB LOGGER "../logger/*.*")

# Main executable
add_executable(${PROJECT_NAME} user_1.cpp ${SOURCES_COMMUNICATION} ${HEADERS_COMMUNICATION} ${SOCKETS_COMMUNICATIONS} ${LOGGER})

include(../hsm-client/use_hsm_client.cmake)
find_library(HSM_CLIENT_LIB hsm_client_lib PATHS ${HSM_CLIENT_LIB_PATH})
if(HSM_CLIENT_LIB)
    message(STATUS "Found hsm_client_lib: ${HSM_CLIENT_LIB}")
    target_link_libraries(${PROJECT_NAME} PRIVATE ${HSM_CLIENT_LIB} ${HSM_CLIENT_LIB_DEPS})
    target_include_directories(${PROJECT_NAME} PRIVATE ${HSM_CLIENT_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Could not find hsm_client_lib at ${HSM_CLIENT_LIB_PATH}")
endif()