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

include(${CMAKE_SOURCE_DIR}/../hsm-client/use_hsm_client.cmake)
configure_grpc_protos()

# Main executable
add_executable(${PROJECT_NAME} user_1.cpp ${SOURCES_COMMUNICATION} ${HEADERS_COMMUNICATION} ${SOCKETS_COMMUNICATIONS} ${LOGGER}   ${HSM_CLIENT_SRCS})
link_hsm_client_libraries(${PROJECT_NAME})

add_proto_dependencies(${PROJECT_NAME})