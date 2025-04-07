#include <iostream>
#include "MPointer.h"

int main() {
  try {
    // Inicializa la conexión al Memory Manager (asegúrate de que el servidor esté en ejecución)
    MPointer<int>::Init("localhost:50051");

    // Crea un nuevo MPointer para un int
    MPointer<int> myPtr = MPointer<int>::New();

    // Asigna un valor remoto (esto envía la operación Set al servidor)
    *myPtr = 42;

    // Recupera el valor remoto (se invoca la conversión a int)
    int value = myPtr;
    std::cout << "Valor obtenido: " << value << std::endl;
  }
  catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
  }
  return 0;
}
