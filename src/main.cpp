#include "MemoryManager.h"
#include "GarbageCollector.h"
#include "MemoryManagerServiceImpl.h"

#include <grpcpp/grpcpp.h>
#include <iostream>
#include <cstring>
#include <cstdlib>

// Estructura para almacenar la configuración de línea de comandos
struct Config {
    int port;
    size_t memSizeMB;
    std::string dumpFolder;
};

Config parseArguments(int argc, char* argv[]) {
    Config config = {0, 0, ""};
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            config.port = std::atoi(argv[++i]);
        } else if (strcmp(argv[i], "--memsize") == 0 && i + 1 < argc) {
            config.memSizeMB = std::atoi(argv[++i]);
        } else if (strcmp(argv[i], "--dumpFolder") == 0 && i + 1 < argc) {
            config.dumpFolder = argv[++i];
        }
    }
    return config;
}

int main(int argc, char* argv[]) {
    Config config = parseArguments(argc, argv);
    if (config.port == 0 || config.memSizeMB == 0 || config.dumpFolder.empty()) {
        std::cerr << "Uso: ./mem-mgr --port <PORT> --memsize <SIZE_MB> --dumpFolder <FOLDER>" << std::endl;
        return EXIT_FAILURE;
    }

    size_t totalSize = config.memSizeMB * 1024 * 1024; // Convertir MB a bytes
    MemoryManager memoryManager(totalSize, config.dumpFolder);

    // Inicia el Garbage Collector (cada 1000 ms)
    GarbageCollector gc(&memoryManager, 1000);
    gc.start();

    // Configura y arranca el servidor GRPC
    std::string serverAddress = "0.0.0.0:" + std::to_string(config.port);
    MemoryManagerServiceImpl service(&memoryManager);

    grpc::ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Servidor iniciado en " << serverAddress << std::endl;

    server->Wait();

    gc.stop();

    return EXIT_SUCCESS;
}
