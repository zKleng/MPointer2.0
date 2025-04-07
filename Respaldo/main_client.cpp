#include <iostream>
#include <memory>
#include "MemoryManagerClient.h"
#include <grpcpp/grpcpp.h>

int main(int argc, char** argv) {
  // Suponiendo que el Memory Manager se ejecuta en localhost:50051
  std::string target_str = "localhost:50051";
  MemoryManagerClient client(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));

  try {
    // Crear un bloque de memoria para, por ejemplo, un int (tamaño 4 bytes)
    std::string id = client.Create(4, "int");
    std::cout << "Bloque creado con id: " << id << std::endl;

    // Establecer el valor "42" en el bloque
    client.Set(id, "42");
    std::cout << "Valor 42 almacenado en el bloque " << id << std::endl;

    // Obtener el valor almacenado
    std::string value = client.Get(id);
    std::cout << "Valor obtenido: " << value << std::endl;

    // Incrementar y luego decrementar el refcount
    client.IncreaseRefCount(id);
    std::cout << "RefCount incrementado para el bloque " << id << std::endl;

    client.DecreaseRefCount(id);
    std::cout << "RefCount decrementado para el bloque " << id << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Excepción: " << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
