cmake_minimum_required(VERSION 3.10)
project(ImageProcessing)

set(CMAKE_CXX_STANDARD 17)

# adding libraries
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

include_directories(include ../logger ../communication/include ../communication/sockets)

# make the library with the src files
file(GLOB SOURCES "src/*.cpp" "../logger/*.cpp" "tests/utils.cpp" "../communication/src/*.cpp" "../communication/sockets/*.cpp")
add_library(ImageProcessingLib ${SOURCES})

# create CommunicationLib library
add_library(CommunicationLib STATIC ../communication/src/communication.cpp ../communication/src/client_connection.cpp ../communication/src/message.cpp ../communication/src/packet.cpp ../communication/src/bus_manager.cpp ../communication/src/server_connection.cpp ../logger/logger.cpp)

configure_file( ${CMAKE_BINARY_DIR}/config.json COPYONLY)
# create test executable
add_executable(runTests tests/main.cpp tests/test_serialize.cpp tests/test_detector.cpp tests/test_dynamic_tracker.cpp tests/test_distance.cpp tests/test_manager.cpp tests/test_velocity.cpp)
target_link_libraries(runTests ImageProcessingLib ${OpenCV_LIBS} ${GTEST_LIBRARIES} pthread CommunicationLib)

#adding tests
enable_testing()
add_test(NAME runTests COMMAND runTests)

add_executable(runMain src/main.cpp)
target_link_libraries(runMain ImageProcessingLib CommunicationLib ${OpenCV_LIBS})

include(../hsm-client/use_hsm_client.cmake)
find_library(HSM_CLIENT_LIB hsm_client_lib PATHS ${HSM_CLIENT_LIB_PATH})
if(HSM_CLIENT_LIB)
    message(STATUS "Found hsm_client_lib: ${HSM_CLIENT_LIB}")
    target_link_libraries(runTests  ${HSM_CLIENT_LIB} ${HSM_CLIENT_LIB_DEPS})
    include_directories(runTests PRIVATE ${HSM_CLIENT_INCLUDE_DIRS})
    target_link_libraries(runMain  ${HSM_CLIENT_LIB} ${HSM_CLIENT_LIB_DEPS})
    include_directories(runMain  PRIVATE ${HSM_CLIENT_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Could not find hsm_client_lib at ${HSM_CLIENT_LIB_PATH}")
endif()


set(LOG_LEVEL "INFO" CACHE STRING "Set the log level")
if (LOG_LEVEL STREQUAL "DEBUG")
    add_definitions(-DLOG_LEVEL=logger::LogLevel::DEBUG)
elseif (LOG_LEVEL STREQUAL "INFO")
    add_definitions(-DLOG_LEVEL=logger::LogLevel::INFO)
else()
    message(FATAL_ERROR "Invalid LOG_LEVEL specified!")
endif()
