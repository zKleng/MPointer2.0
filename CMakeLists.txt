cmake_minimum_required(VERSION 3.10)
project(MPointers2.0 LANGUAGES CXX)

# Usamos C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Buscar las dependencias de Protobuf y gRPC (abseil no se menciona explícitamente)
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

# Directorios de cabeceras
include_directories(
  ${Protobuf_INCLUDE_DIRS}
  ${gRPC_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}   # para los archivos generados .pb.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Buscar todos los archivos .proto en la carpeta proto
file(GLOB PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto")

# Variables para almacenar los archivos generados
set(GENERATED_PROTO_SRCS "")
set(GENERATED_PROTO_HDRS "")

# Por cada archivo .proto se genera código C++ y gRPC
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

# Buscar los archivos fuente C++ del proyecto en la carpeta src
file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Crear el ejecutable (se incluye el código generado de los .proto)
add_executable(mem-mgr ${SRC_FILES} ${GENERATED_PROTO_SRCS})

# Enlazar con las bibliotecas necesarias
target_link_libraries(mem-mgr
  PUBLIC
    ${Protobuf_LIBRARIES}
    gRPC::grpc++
)
