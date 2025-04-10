#ifndef DUMP_MANAGER_H
#define DUMP_MANAGER_H

#include <string>
#include <unordered_map>
#include "MemoryManager.h"  // Ahora MemoryBlock está definido

class DumpManager {
public:
    // Función de volcado "compacto" que usa la información de cada bloque.
    static void dump(const std::unordered_map<uint32_t, MemoryBlock>& blocks,
                     const char* memory,
                     size_t totalSize,
                     const std::string &dumpFolder);
};

#endif // DUMP_MANAGER_H
