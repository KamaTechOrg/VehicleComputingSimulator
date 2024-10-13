cmake_minimum_required(VERSION 3.10)
project("MainControl" LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Find and include bson library
find_package(PkgConfig REQUIRED)
pkg_check_modules(BSON REQUIRED libbson-1.0)
include_directories(${BSON_INCLUDE_DIRS})

# Collect source files
file(GLOB SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")


# # Path to local communication directory
# set(HSM_CLIENT_PATH "${CMAKE_SOURCE_DIR}/../hsm-client")

# # Add communication
# add_subdirectory(${HSM_CLIENT_PATH} ${CMAKE_BINARY_DIR}/hsm-client)

file(GLOB SOURCES_COMMUNICATION "../communication/src/*.cpp")
file(GLOB HEADERS_COMMUNICATION "../communication/include/*.h")
file(GLOB SOCKETS_COMMUNICATIONS "../communication/sockets/*.*" )

file(GLOB PARSER "../parser_json/src/*.*")

file(GLOB LOGGER "../logger/*.*")

include(${CMAKE_SOURCE_DIR}/../hsm-client/use_hsm_client.cmake)
configure_grpc_protos()

# Main executable
add_executable(${PROJECT_NAME} ${SOURCES} ${PARSER} ${SOURCES_COMMUNICATION} ${HEADERS_COMMUNICATION} ${SOCKETS_COMMUNICATIONS} ${LOGGER}  ${HSM_CLIENT_SRCS} src/main.cpp )
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE ${BSON_LIBRARIES} )

link_hsm_client_libraries(${PROJECT_NAME})

add_proto_dependencies(${PROJECT_NAME})


# Test executable, including additional source files
file(GLOB TEST_SOURCES "test/*.cpp")
add_executable(RunTests ${SOURCES} ${PARSER} ${SOURCES_COMMUNICATION} ${HEADERS_COMMUNICATION} ${SOCKETS_COMMUNICATIONS} ${LOGGER} ${TEST_SOURCES}   ${HSM_CLIENT_SRCS})
target_include_directories(RunTests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(RunTests PRIVATE ${BSON_LIBRARIES} ${GTEST_LIBRARIES} pthread)
link_hsm_client_libraries(RunTests)

add_proto_dependencies(RunTests)