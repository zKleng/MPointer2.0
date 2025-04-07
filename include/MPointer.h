#ifndef MPOINTER_H
#define MPOINTER_H

#include <string>
#include <stdexcept>
#include <cstring>
#include <typeinfo>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "MemoryManagerClient.h"  // Asegúrate de que este archivo esté en include/

// La clase MPointer encapsula un "puntero remoto" a un bloque de memoria administrado
// por el Memory Manager a través de gRPC.
template <typename T>
class MPointer {
public:
  // Objeto proxy que permite la asignación y lectura mediante el operador *.
  class Proxy {
  public:
    Proxy(MPointer<T>& ptr) : mp(ptr) {}
    
    // Conversión a T para obtener el valor remoto.
    operator T() const {
      return static_cast<T>(mp);  // Utiliza el operator T() ya implementado
    }
    
    // Asignación: se envía el nuevo valor al Memory Manager.
    Proxy& operator=(const T &newValue) {
      mp = newValue; // Invoca MPointer<T>::operator=(const T&)
      return *this;
    }
    
  private:
    MPointer<T>& mp;
  };

  // Inicializa la conexión al Memory Manager.
  // Debe llamarse al inicio del programa cliente.
  // serverAddress: "host:puerto" (ejemplo: "localhost:50051")
  static void Init(const std::string &serverAddress) {
    client_ = std::make_shared<MemoryManagerClient>(
                grpc::CreateChannel(serverAddress, grpc::InsecureChannelCredentials()));
  }

  // Método estático para crear un nuevo MPointer para el tipo T.
  // Solicita al Memory Manager un bloque de memoria del tamaño de T.
  static MPointer<T> New() {
    if (!client_) {
      throw std::runtime_error("MemoryManagerClient no está inicializado. Llama a MPointer<T>::Init(serverAddress) primero.");
    }
    size_t size = sizeof(T);
    // Utilizamos el nombre retornado por typeid para indicar el tipo.
    std::string typeName = typeid(T).name();
    // Se solicita al Memory Manager que cree un bloque.
    std::string id = client_->Create(size, typeName);
    return MPointer<T>(id);
  }

  // Operador de conversión a T: se invoca cuando se quiere obtener el valor remoto.
  // Realiza la petición Get al Memory Manager.
  operator T() const {
    if (!client_) {
      throw std::runtime_error("MemoryManagerClient no está inicializado.");
    }
    std::string remoteValue = client_->Get(id_);
    if (remoteValue.size() != sizeof(T)) {
      throw std::runtime_error("El tamaño del valor remoto no coincide con sizeof(T).");
    }
    T localValue;
    std::memcpy(&localValue, remoteValue.data(), sizeof(T));
    return localValue;
  }

  // Sobrecarga del operador de asignación para asignar un valor de tipo T al MPointer.
  // Se empaqueta el valor en bytes y se envía al Memory Manager.
  void operator=(const T &newValue) {
    if (!client_) {
      throw std::runtime_error("MemoryManagerClient no está inicializado.");
    }
    std::string bytes;
    bytes.resize(sizeof(T));
    std::memcpy(&bytes[0], &newValue, sizeof(T));
    client_->Set(id_, bytes);
  }

  // Sobrecarga del operador de dirección (&) para obtener el ID remoto.
  std::string operator&() const {
    return id_;
  }

  // Sobrecarga del operador de dereferenciación para lectura/escritura.
  Proxy operator*() {
    return Proxy(*this);
  }
  
  // Versión const para solo lectura.
  T operator*() const {
    return static_cast<T>(*this);
  }

  // Constructor de copia: incrementa el contador de referencias en el servidor.
  MPointer(const MPointer<T>& other)
      : id_(other.id_) {
    if (client_) {
      client_->IncreaseRefCount(id_);
    }
  }

  // Operador de asignación de copia.
  MPointer<T>& operator=(const MPointer<T>& other) {
    if (this != &other) {
      if (!id_.empty() && client_) {
        client_->DecreaseRefCount(id_);
      }
      id_ = other.id_;
      if (client_) {
        client_->IncreaseRefCount(id_);
      }
    }
    return *this;
  }

  // Destructor: decrementa el contador de referencias en el servidor.
  ~MPointer() {
    if (!id_.empty() && client_) {
      client_->DecreaseRefCount(id_);
    }
  }

private:
  // Constructor privado para crear un MPointer a partir de un id remoto.
  explicit MPointer(const std::string &id) : id_(id) {}

  std::string id_;    // Identificador remoto del bloque de memoria.

  // Puntero estático compartido al MemoryManagerClient.
  static std::shared_ptr<MemoryManagerClient> client_;
};

// Definición del miembro estático.
template <typename T>
std::shared_ptr<MemoryManagerClient> MPointer<T>::client_ = nullptr;

#endif // MPOINTER_H
