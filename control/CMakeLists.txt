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

#----------------------------------------------------

find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
# # Find OpenCV package
# find_package(OpenCV REQUIRED)
# Specify the path to the proto files
set(PROTO_FILES
    ${CMAKE_SOURCE_DIR}/../hsm-client/proto/encryption.proto 
)
# Paths to the protoc and grpc_cpp_plugin binaries
set(PROTOC_PATH "/usr/local/bin/protoc")
set(GRPC_CPP_PLUGIN_PATH "/usr/local/bin/grpc_cpp_plugin")
# Specify output directory for generated files
set(PROTO_GEN_DIR "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})
# Generate C++ source files from proto files
foreach(proto_file ${PROTO_FILES})
    get_filename_component(proto_name ${proto_file} NAME_WE)
    
    # Protobuf C++ source files
    add_custom_command(
        OUTPUT ${PROTO_GEN_DIR}/${proto_name}.pb.cc ${PROTO_GEN_DIR}/${proto_name}.pb.h
        COMMAND ${PROTOC_PATH} --cpp_out=${PROTO_GEN_DIR} --proto_path=${CMAKE_SOURCE_DIR}/proto --proto_path=${CMAKE_SOURCE_DIR}/../hsm-client/proto ${proto_file}
        DEPENDS ${proto_file}
        COMMENT "Generating protobuf code for ${proto_file}"
    )
    
    # gRPC C++ source files
    add_custom_command(
        OUTPUT ${PROTO_GEN_DIR}/${proto_name}.grpc.pb.cc ${PROTO_GEN_DIR}/${proto_name}.grpc.pb.h
        COMMAND ${PROTOC_PATH} --grpc_out=${PROTO_GEN_DIR} --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_PATH} --proto_path=${CMAKE_SOURCE_DIR}/proto --proto_path=${CMAKE_SOURCE_DIR}/../hsm-client/proto ${proto_file}
        DEPENDS ${proto_file}
        COMMENT "Generating gRPC code for ${proto_file}"
    )
    
    list(APPEND PROTO_SRCS ${PROTO_GEN_DIR}/${proto_name}.pb.cc ${PROTO_GEN_DIR}/${proto_name}.grpc.pb.cc)
    list(APPEND PROTO_HDRS ${PROTO_GEN_DIR}/${proto_name}.pb.h ${PROTO_GEN_DIR}/${proto_name}.grpc.pb.h)
endforeach()

# Include the generated files directory
include_directories(${PROTO_GEN_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../hsm-client/include)
# Include directories for protobuf, gRPC, and OpenCV
include_directories(${Protobuf_INCLUDE_DIRS} ${GRPC_INCLUDE_DIRS})
# Add the executable
set(ADDITIONAL_SRC_FILES
    ${CMAKE_SOURCE_DIR}/../hsm-client/src/crypto_api.cpp
    ${CMAKE_SOURCE_DIR}/../hsm-client/src/general.cpp
    ${CMAKE_SOURCE_DIR}/../logger/logger.cpp
    ${PROTO_SRCS}
)
#----------------------------------------------------------------
# # Path to local communication directory
# set(HSM_CLIENT_PATH "${CMAKE_SOURCE_DIR}/../hsm-client")

# # Add communication
# add_subdirectory(${HSM_CLIENT_PATH} ${CMAKE_BINARY_DIR}/hsm-client)

file(GLOB SOURCES_COMMUNICATION "../communication/src/*.cpp")
file(GLOB HEADERS_COMMUNICATION "../communication/include/*.h")
file(GLOB SOCKETS_COMMUNICATIONS "../communication/sockets/*.*" )

file(GLOB PARSER "../parser_json/src/*.*")

file(GLOB LOGGER "../logger/*.*")

# Main executable
add_executable(${PROJECT_NAME} ${SOURCES} ${PARSER} ${SOURCES_COMMUNICATION} ${HEADERS_COMMUNICATION} ${SOCKETS_COMMUNICATIONS} ${LOGGER} ${ADDITIONAL_SRC_FILES} src/main.cpp )
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE ${BSON_LIBRARIES} ${Protobuf_LIBRARIES} gRPC::grpc++)


#----
add_custom_target(proto_gen ALL
    DEPENDS ${PROTO_SRCS} ${PROTO_HDRS}
    COMMENT "Generating protobuf and gRPC code"
)
# Add dependencies to ensure proper build order
add_dependencies(${PROJECT_NAME} proto_gen)

#----


# Test executable, including additional source files
file(GLOB TEST_SOURCES "test/*.cpp")
add_executable(RunTests ${SOURCES} ${PARSER} ${SOURCES_COMMUNICATION} ${HEADERS_COMMUNICATION} ${SOCKETS_COMMUNICATIONS} ${LOGGER} ${TEST_SOURCES} ${ADDITIONAL_SRC_FILES})
target_include_directories(RunTests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(RunTests PRIVATE ${BSON_LIBRARIES} ${GTEST_LIBRARIES} pthread ${Protobuf_LIBRARIES} gRPC::grpc++)