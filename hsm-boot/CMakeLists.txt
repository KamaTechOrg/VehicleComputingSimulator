cmake_minimum_required(VERSION 3.10)
project("hsmBoot")

include(${CMAKE_SOURCE_DIR}/../hsm-client/use_hsm_client.cmake)
configure_grpc_protos()
add_executable(${PROJECT_NAME} main.cpp  ${HSM_CLIENT_SRCS})
target_link_libraries(${PROJECT_NAME} PRIVATE )
link_hsm_client_libraries(${PROJECT_NAME})

add_proto_dependencies(${PROJECT_NAME})
