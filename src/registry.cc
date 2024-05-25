#include "registry.h"
#include "logging_buffer.h"

std::map<RequestType, RequestHandlerFactory> Registry::request_handlers_map;


void Registry::RegisterRequestHandler(const RequestType& type, RequestHandlerFactory factory){
    request_handlers_map[type] = factory;
}

RequestHandler* Registry::GetRequestHandler(const RequestType& type, LoggingBuffer* logging_buffer){
    auto it = request_handlers_map.find(type);
    if(it == request_handlers_map.end()){
        return nullptr;
    }
    return it->second(logging_buffer);
}
