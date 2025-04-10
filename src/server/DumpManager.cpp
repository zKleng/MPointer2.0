#include "DumpManager.h"
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>

void DumpManager::dump(const std::unordered_map<uint32_t, MemoryBlock>& blocks,
                       const char* memory,
                       size_t totalSize,
                       const std::string &dumpFolder) {
    // Asegúrate de que la carpeta exista:
    mkdir(dumpFolder.c_str(), 0777);

    // Obtener fecha y hora actual con milisegundos
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << dumpFolder << "/dump_"
        << std::put_time(&tm, "%Y%m%d_%H%M%S")
        << "_" << std::setfill('0') << std::setw(3) << ms.count() << ".txt";

    std::ofstream ofs(oss.str(), std::ios::out);
    if (ofs.is_open()) {
        ofs << "Estado de la memoria (total " << totalSize << " bytes):\n";
        // Volcar solo la información de los bloques asignados.
        for (const auto& kv : blocks) {
            const MemoryBlock &block = kv.second;
            ofs << "Block ID: " << block.id << ", Offset: " << block.offset
                << ", Size: " << block.size << ", Type: " << block.type
                << ", RefCount: " << block.refCount << "\n";
            // Volcamos el contenido del bloque en hexadecimal.
            ofs << "Contenido: ";
            for (size_t i = 0; i < block.size; i++) {
                ofs << std::hex << std::setw(2) << std::setfill('0')
                    << (int)(unsigned char)memory[block.offset + i] << " ";
            }
            ofs << "\n\n";
        }
        ofs.close();
    }
}
