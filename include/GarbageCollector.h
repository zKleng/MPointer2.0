#ifndef GARBAGE_COLLECTOR_H
#define GARBAGE_COLLECTOR_H

#include "MemoryManager.h"
#include <thread>
#include <atomic>

class GarbageCollector {
public:
    GarbageCollector(MemoryManager* mm, int intervalMs);
    ~GarbageCollector();

    void start();
    void stop();

private:
    void run();

    MemoryManager* memoryManager;
    int intervalMs;  // Intervalo en milisegundos entre revisiones
    std::atomic<bool> running;
    std::thread gcThread;
};

#endif // GARBAGE_COLLECTOR_H
