#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <mutex>
#include <cstddef>

// Estructura para la metadata de cada bloque de memoria
struct MemoryBlock {
    uint32_t id;
    size_t offset;
    size_t size;
    std::string type;
    int refCount;
};

class MemoryManager {
public:
    MemoryManager(size_t totalSize, const std::string &dumpFolder);
    ~MemoryManager();

    // Métodos para gestionar la memoria
    uint32_t create(size_t size, const std::string &type);
    bool set(uint32_t id, const void* value, size_t valueSize);
    bool get(uint32_t id, void* outValue, size_t valueSize);
    bool increaseRefCount(uint32_t id);
    bool decreaseRefCount(uint32_t id);

    // Acceso a los bloques (utilizado por el garbage collector)
    std::unordered_map<uint32_t, MemoryBlock>& getBlocks();

    // Libera un bloque y limpia la memoria
    void freeBlock(uint32_t id);

    // Genera un volcado (dump) del estado de la memoria
    void dumpMemory();

    // Acceso al bloque de memoria y su tamaño
    char* getMemory() const { return memoryBlock; }
    size_t getTotalSize() const { return totalSize; }

private:
    // Función interna para buscar un espacio libre
    size_t findFreeBlock(size_t size);

    char* memoryBlock;         // Bloque de memoria reservado con malloc
    size_t totalSize;          // Tamaño total en bytes
    std::string dumpFolder;    // Carpeta donde se guardarán los dumps
    uint32_t nextId;           // Para generar IDs únicos
    size_t nextFreeOffset;     // Apuntador al siguiente offset libre
    std::unordered_map<uint32_t, MemoryBlock> blocks;
    std::mutex mtx;            // Protección en operaciones concurrentes
};

#endif // MEMORY_MANAGER_H
