#include "GarbageCollector.h"
#include <chrono>
#include <thread>
#include <iostream>

GarbageCollector::GarbageCollector(MemoryManager* mm, int intervalMs)
    : memoryManager(mm), intervalMs(intervalMs), running(false)
{}

GarbageCollector::~GarbageCollector() {
    stop();
}

void GarbageCollector::start() {
    running = true;
    gcThread = std::thread(&GarbageCollector::run, this);
}

void GarbageCollector::stop() {
    running = false;
    if (gcThread.joinable())
        gcThread.join();
}

void GarbageCollector::run() {
    while (running) {
        // Recorre los bloques y libera aquellos con refCount <= 0
        {
            for (auto it = memoryManager->getBlocks().begin(); it != memoryManager->getBlocks().end(); ) {
                if (it->second.refCount <= 0) {
                    uint32_t id = it->first;
                    it = memoryManager->getBlocks().erase(it);
                    std::cout << "GarbageCollector: Liberado bloque con ID " << id << std::endl;
                } else {
                    ++it;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
    }
}
