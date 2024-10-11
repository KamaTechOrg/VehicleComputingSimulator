# Function to generate protobuf and gRPC code
function(configure_grpc_protos)
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
    # Hardcoded proto files and paths
    set(PROTO_FILES
        ${CMAKE_SOURCE_DIR}/../hsm-client/proto/encryption.proto
    )
    set(PROTOC_PATH "/usr/local/bin/protoc")
    set(GRPC_CPP_PLUGIN_PATH "/usr/local/bin/grpc_cpp_plugin")
    set(PROTO_GEN_DIR "${CMAKE_BINARY_DIR}/generated")
    
    # Create the output directory for generated files
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

    # Export the generated source and header file lists


    # Create a custom target to trigger protobuf/gRPC generation
    add_custom_target(proto_gen ALL
        DEPENDS ${PROTO_SRCS} ${PROTO_HDRS}
        COMMENT "Generating protobuf and gRPC code"
    )

   include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../hsm-client/include)
include_directories(${Protobuf_INCLUDE_DIRS} ${GRPC_INCLUDE_DIRS})
set(HSM_CLIENT_SRCS
    ${CMAKE_SOURCE_DIR}/../hsm-client/src/crypto_api.cpp
    ${CMAKE_SOURCE_DIR}/../hsm-client/src/general.cpp
    ${CMAKE_SOURCE_DIR}/../logger/logger.cpp
    ${PROTO_SRCS}
)

set(PROTO_HDRS ${PROTO_HDRS} PARENT_SCOPE)
set(HSM_CLIENT_SRCS ${HSM_CLIENT_SRCS} PARENT_SCOPE)
endfunction()

# Function to link required libraries to a target
function(link_hsm_client_libraries target_name)
    target_link_libraries(${target_name} PRIVATE 
        ${Protobuf_LIBRARIES} 
        gRPC::grpc++
    )
endfunction()

# Function to add dependencies to the target
function(add_proto_dependencies target_name)
    add_dependencies(${target_name} proto_gen)
endfunction()

