#pragma once
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <cstring>
#include <iostream>
#include <string>

// Declaración de MemoryBlock (se usa en varios lugares)
struct MemoryBlock {
    uint32_t id;
    size_t offset;
    size_t size;
    std::string type;
    int refCount;
};

class MemoryManager {
public:
    // Constructor y destructor
    MemoryManager(size_t totalSize, const std::string &dumpFolder);
    ~MemoryManager();

    // Métodos públicos
    uint32_t create(size_t size, const std::string &type);
    bool set(uint32_t id, const void* value, size_t valueSize);
    bool get(uint32_t id, void* outValue, size_t valueSize);
    bool increaseRefCount(uint32_t id);
    bool decreaseRefCount(uint32_t id);
    void defragment();
    void dumpMemory();
    
    // Para volcado u otros usos
    std::unordered_map<uint32_t, MemoryBlock>& getBlocks();

private:
    // Método privado: declara findFreeBlock
    size_t findFreeBlock(size_t size);

    // Método privado para liberar un bloque
    void freeBlock(uint32_t id);

    // Datos miembro
    char* memoryBlock;
    size_t totalSize;       // Total de memoria reservada
    std::string dumpFolder;
    size_t nextFreeOffset;  // Siguiente offset libre dentro de memoryBlock
    uint32_t nextId;
    std::unordered_map<uint32_t, MemoryBlock> blocks;
    std::mutex mtx;
};

