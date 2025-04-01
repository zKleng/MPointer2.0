#include "MemoryManager.h"
#include "DumpManager.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>

MemoryManager::MemoryManager(size_t totalSize, const std::string &dumpFolder)
    : totalSize(totalSize), dumpFolder(dumpFolder), nextId(1), nextFreeOffset(0)
{
    memoryBlock = (char*)malloc(totalSize);
    if (!memoryBlock) {
        std::cerr << "Error reservando memoria." << std::endl;
        exit(EXIT_FAILURE);
    }
    memset(memoryBlock, 0, totalSize);
}

MemoryManager::~MemoryManager() {
    free(memoryBlock);
}

uint32_t MemoryManager::create(size_t size, const std::string &type) {
    std::lock_guard<std::mutex> lock(mtx);
    size_t offset = findFreeBlock(size);
    if (offset == SIZE_MAX) {
        std::cerr << "No hay suficiente memoria para asignar el bloque solicitado." << std::endl;
        return 0; // Error: sin espacio
    }
    uint32_t id = nextId++;
    MemoryBlock block { id, offset, size, type, 1 };
    blocks[id] = block;
    nextFreeOffset = std::max(nextFreeOffset, offset + size);
    dumpMemory();
    return id;
}

bool MemoryManager::set(uint32_t id, const void* value, size_t valueSize) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = blocks.find(id);
    if (it == blocks.end()) {
        return false;
    }
    MemoryBlock &block = it->second;
    if (valueSize != block.size) {
        std::cerr << "Tamaño de valor incorrecto para el bloque." << std::endl;
        return false;
    }
    memcpy(memoryBlock + block.offset, value, valueSize);
    dumpMemory();
    return true;
}

bool MemoryManager::get(uint32_t id, void* outValue, size_t valueSize) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = blocks.find(id);
    if (it == blocks.end()) {
        return false;
    }
    MemoryBlock &block = it->second;
    if (valueSize != block.size) {
        std::cerr << "Tamaño de salida incorrecto para el bloque." << std::endl;
        return false;
    }
    memcpy(outValue, memoryBlock + block.offset, valueSize);
    return true;
}

bool MemoryManager::increaseRefCount(uint32_t id) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = blocks.find(id);
    if (it == blocks.end()) {
        return false;
    }
    it->second.refCount++;
    dumpMemory();
    return true;
}

bool MemoryManager::decreaseRefCount(uint32_t id) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = blocks.find(id);
    if (it == blocks.end()) {
        return false;
    }
    it->second.refCount--;
    dumpMemory();
    if (it->second.refCount <= 0) {
        freeBlock(id);
    }
    return true;
}

void MemoryManager::freeBlock(uint32_t id) {
    auto it = blocks.find(id);
    if (it != blocks.end()) {
        // Opcional: limpiar la memoria del bloque liberado
        memset(memoryBlock + it->second.offset, 0, it->second.size);
        blocks.erase(it);
        dumpMemory();
    }
}

size_t MemoryManager::findFreeBlock(size_t size) {
    // Método simple: utiliza el nextFreeOffset
    if (nextFreeOffset + size <= totalSize) {
        return nextFreeOffset;
    }
    // Si no hay espacio al final, se podría implementar búsqueda en huecos liberados
    return SIZE_MAX;
}

void MemoryManager::dumpMemory() {
    DumpManager::dump(memoryBlock, totalSize, dumpFolder);
}

std::unordered_map<uint32_t, MemoryBlock>& MemoryManager::getBlocks() {
    return blocks;
}
