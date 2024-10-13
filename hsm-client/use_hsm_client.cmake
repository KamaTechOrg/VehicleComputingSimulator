set(HSM_CLIENT_DIR "../hsm-client")
set(HSM_CLIENT_LIB_PATH ${HSM_CLIENT_DIR}/build/lib)
set(HSM_CLIENT_INCLUDE_DIRS
    ${HSM_CLIENT_DIR}/include
    ${PROTO_GEN_DIR}
    ${Protobuf_INCLUDE_DIRS}
    ${GRPC_INCLUDE_DIRS}
    ${HSM_CLIENT_DIR}/../logger)

find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
set(HSM_CLIENT_LIB_DEPS ${Protobuf_LIBRARIES} gRPC::grpc++)