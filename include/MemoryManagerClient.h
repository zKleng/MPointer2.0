#ifndef MEMORY_MANAGER_CLIENT_H
#define MEMORY_MANAGER_CLIENT_H

#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "memory_manager.grpc.pb.h"  // Código generado a partir de tu .proto

class MemoryManagerClient {
public:
  // Constructor que recibe el canal gRPC
  MemoryManagerClient(std::shared_ptr<grpc::Channel> channel);

  // Solicita la creación de un bloque de memoria de tamaño "size" y tipo "type"
  // Devuelve el id asignado (convertido a string)
  std::string Create(size_t size, const std::string &type);

  // Envía una petición para almacenar "value" (como string) en el bloque identificado por id
  void Set(const std::string &id, const std::string &value);

  // Envía una petición para obtener el valor almacenado en el bloque identificado por id
  std::string Get(const std::string &id);

  // Incrementa el contador de referencias para el bloque id
  void IncreaseRefCount(const std::string &id);

  // Decrementa el contador de referencias para el bloque id
  void DecreaseRefCount(const std::string &id);

private:
  // Usamos el stub del servicio generado: MemoryManagerService
  std::unique_ptr<memorymanager::MemoryManagerService::Stub> stub_;
};

#endif  // MEMORY_MANAGER_CLIENT_H

