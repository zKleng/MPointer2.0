#ifndef DUMP_MANAGER_H
#define DUMP_MANAGER_H

#include <string>

class DumpManager {
public:
    // Genera un archivo dump con el estado de la memoria
    static void dump(const char* memory, size_t size, const std::string &dumpFolder);
};

#endif // DUMP_MANAGER_H
