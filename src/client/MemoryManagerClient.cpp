#include "MemoryManagerClient.h"
#include <stdexcept>
#include <cstdlib>   // Para std::stoul
#include <string>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

MemoryManagerClient::MemoryManagerClient(std::shared_ptr<Channel> channel)
    : stub_(memorymanager::MemoryManagerService::NewStub(channel)) {}

std::string MemoryManagerClient::Create(size_t size, const std::string &type) {
    memorymanager::CreateRequest request;
    memorymanager::CreateResponse response;
    ClientContext context;

    request.set_size(static_cast<uint32_t>(size));
    request.set_type(type);

    Status status = stub_->Create(&context, request, &response);
    if (!status.ok()) {
        throw std::runtime_error("Create RPC failed: " + status.error_message());
    }
    // Convertir el id (uint32_t) a std::string
    return std::to_string(response.id());
}

void MemoryManagerClient::Set(const std::string &id, const std::string &value) {
    memorymanager::SetRequest request;
    memorymanager::SetResponse response;
    ClientContext context;

    // Convertir el id de string a uint32_t
    uint32_t numeric_id = static_cast<uint32_t>(std::stoul(id));
    request.set_id(numeric_id);
    request.set_value(value); // El campo value es de tipo bytes, se usa std::string

    Status status = stub_->Set(&context, request, &response);
    if (!status.ok()) {
        throw std::runtime_error("Set RPC failed: " + status.error_message());
    }
}

std::string MemoryManagerClient::Get(const std::string &id) {
    memorymanager::GetRequest request;
    memorymanager::GetResponse response;
    ClientContext context;

    uint32_t numeric_id = static_cast<uint32_t>(std::stoul(id));
    request.set_id(numeric_id);

    Status status = stub_->Get(&context, request, &response);
    if (!status.ok()) {
        throw std::runtime_error("Get RPC failed: " + status.error_message());
    }
    return response.value();  // value es bytes, se maneja como std::string
}

void MemoryManagerClient::IncreaseRefCount(const std::string &id) {
    memorymanager::RefCountRequest request;
    memorymanager::RefCountResponse response;
    ClientContext context;

    uint32_t numeric_id = static_cast<uint32_t>(std::stoul(id));
    request.set_id(numeric_id);

    Status status = stub_->IncreaseRefCount(&context, request, &response);
    if (!status.ok()) {
        throw std::runtime_error("IncreaseRefCount RPC failed: " + status.error_message());
    }
}

void MemoryManagerClient::DecreaseRefCount(const std::string &id) {
    memorymanager::RefCountRequest request;
    memorymanager::RefCountResponse response;
    ClientContext context;

    uint32_t numeric_id = static_cast<uint32_t>(std::stoul(id));
    request.set_id(numeric_id);

    Status status = stub_->DecreaseRefCount(&context, request, &response);
    if (!status.ok()) {
        throw std::runtime_error("DecreaseRefCount RPC failed: " + status.error_message());
    }
}
