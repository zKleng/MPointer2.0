cmake_minimum_required(VERSION 3.10)
project(MPointers2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

include_directories(
    ${Protobuf_INCLUDE_DIRS}
    ${gRPC_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}  # Para .pb.h generados
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Buscar archivos .proto
file(GLOB PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto")

# Para almacenar los archivos generados
set(GENERATED_PROTO_SRCS "")
set(GENERATED_PROTO_HDRS "")

# Generar código para cada .proto
foreach(proto_file ${PROTO_FILES})
  get_filename_component(proto_filename ${proto_file} NAME_WE)
  set(proto_src "${CMAKE_CURRENT_BINARY_DIR}/${proto_filename}.pb.cc")
  set(proto_hdr "${CMAKE_CURRENT_BINARY_DIR}/${proto_filename}.pb.h")
  set(grpc_src "${CMAKE_CURRENT_BINARY_DIR}/${proto_filename}.grpc.pb.cc")
  set(grpc_hdr "${CMAKE_CURRENT_BINARY_DIR}/${proto_filename}.grpc.pb.h")

  add_custom_command(
    OUTPUT ${proto_src} ${proto_hdr} ${grpc_src} ${grpc_hdr}
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --grpc_out ${CMAKE_CURRENT_BINARY_DIR} --cpp_out ${CMAKE_CURRENT_BINARY_DIR}
         --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin
         -I ${CMAKE_CURRENT_SOURCE_DIR}/proto
         ${proto_file}
    DEPENDS ${proto_file}
  )

  list(APPEND GENERATED_PROTO_SRCS ${proto_src} ${grpc_src})
  list(APPEND GENERATED_PROTO_HDRS ${proto_hdr} ${grpc_hdr})
endforeach()

# -----------------------------------------------------------
#           SERVIDOR
# -----------------------------------------------------------
# Agrupa los .cpp del servidor (MemoryManager, DumpManager, etc.)
file(GLOB SERVER_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/server/*.cpp"
)

add_executable(mem-mgr
    ${SERVER_SRC}
    ${GENERATED_PROTO_SRCS}
)

target_link_libraries(mem-mgr
    PUBLIC
        ${Protobuf_LIBRARIES}
        gRPC::grpc++
)

# -----------------------------------------------------------
#           CLIENTE
# -----------------------------------------------------------
# Agrupa los .cpp del cliente (MemoryManagerClient, main_client, etc.)
file(GLOB CLIENT_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/client/*.cpp"
)

add_executable(mem-mgr-client
    ${CLIENT_SRC}
    ${GENERATED_PROTO_SRCS}
)

target_link_libraries(mem-mgr-client
    PUBLIC
        ${Protobuf_LIBRARIES}
        gRPC::grpc++
)
