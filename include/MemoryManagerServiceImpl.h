#ifndef MEMORY_MANAGER_SERVICE_IMPL_H
#define MEMORY_MANAGER_SERVICE_IMPL_H
#include "MemoryManager.h"
#include "memory_manager.grpc.pb.h"  // Generado a partir del proto

class MemoryManagerServiceImpl final : public memorymanager::MemoryManagerService::Service {
public:
    MemoryManagerServiceImpl(MemoryManager* memMgr) : memoryManager(memMgr) {}

    // Implementación de los métodos del servicio GRPC
    grpc::Status Create(grpc::ServerContext* context, const memorymanager::CreateRequest* request,
                          memorymanager::CreateResponse* response) override;

    grpc::Status Set(grpc::ServerContext* context, const memorymanager::SetRequest* request,
                       memorymanager::SetResponse* response) override;

    grpc::Status Get(grpc::ServerContext* context, const memorymanager::GetRequest* request,
                       memorymanager::GetResponse* response) override;

    grpc::Status IncreaseRefCount(grpc::ServerContext* context, const memorymanager::RefCountRequest* request,
                                  memorymanager::RefCountResponse* response) override;

    grpc::Status DecreaseRefCount(grpc::ServerContext* context, const memorymanager::RefCountRequest* request,
                                  memorymanager::RefCountResponse* response) override;

private:
    MemoryManager* memoryManager;
};

#endif // MEMORY_MANAGER_SERVICE_IMPL_H
