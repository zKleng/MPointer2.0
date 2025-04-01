#include "MemoryManagerServiceImpl.h"
#include <cstring>
#include <iostream>

grpc::Status MemoryManagerServiceImpl::Create(grpc::ServerContext* context, const memorymanager::CreateRequest* request,
                                                memorymanager::CreateResponse* response) {
    uint32_t id = memoryManager->create(request->size(), request->type());
    if (id == 0) {
        return grpc::Status(grpc::StatusCode::RESOURCE_EXHAUSTED, "No se pudo asignar memoria.");
    }
    response->set_id(id);
    return grpc::Status::OK;
}

grpc::Status MemoryManagerServiceImpl::Set(grpc::ServerContext* context, const memorymanager::SetRequest* request,
                                             memorymanager::SetResponse* response) {
    bool success = memoryManager->set(request->id(), request->value().data(), request->value().size());
    response->set_success(success);
    return grpc::Status::OK;
}

grpc::Status MemoryManagerServiceImpl::Get(grpc::ServerContext* context, const memorymanager::GetRequest* request,
                                             memorymanager::GetResponse* response) {
    // Se asume que se conoce el tamaño del bloque
    auto& blocks = memoryManager->getBlocks();
    auto it = blocks.find(request->id());
    if (it == blocks.end()) {
        return grpc::Status(grpc::StatusCode::NOT_FOUND, "Bloque no encontrado.");
    }
    size_t blockSize = it->second.size;
    char* buffer = new char[blockSize];
    if (!memoryManager->get(request->id(), buffer, blockSize)) {
        delete[] buffer;
        return grpc::Status(grpc::StatusCode::INTERNAL, "Error al obtener el valor.");
    }
    response->set_value(buffer, blockSize);
    delete[] buffer;
    return grpc::Status::OK;
}

grpc::Status MemoryManagerServiceImpl::IncreaseRefCount(grpc::ServerContext* context, const memorymanager::RefCountRequest* request,
                                                        memorymanager::RefCountResponse* response) {
    bool success = memoryManager->increaseRefCount(request->id());
    response->set_success(success);
    return grpc::Status::OK;
}

grpc::Status MemoryManagerServiceImpl::DecreaseRefCount(grpc::ServerContext* context, const memorymanager::RefCountRequest* request,
                                                        memorymanager::RefCountResponse* response) {
    bool success = memoryManager->decreaseRefCount(request->id());
    response->set_success(success);
    return grpc::Status::OK;
}
