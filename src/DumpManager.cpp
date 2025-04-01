#include "DumpManager.h"
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>

void DumpManager::dump(const char* memory, size_t size, const std::string &dumpFolder) {
    // Verifica o crea la carpeta de dumps si no existe
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
        ofs << "Estado de la memoria (" << size << " bytes):\n";
        for (size_t i = 0; i < size; i++) {
            ofs << std::hex << std::setw(2) << std::setfill('0')
                << (int)(unsigned char)memory[i] << " ";
            if ((i+1) % 16 == 0)
                ofs << "\n";
        }
        ofs.close();
    }
}
