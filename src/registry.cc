#include "registry.h"

std::map<RequestType, RequestHandlerFactory> Registry::request_handlers_map;


void Registry::RegisterRequestHandler(const RequestType& type, RequestHandlerFactory factory){
    request_handlers_map[type] = factory;
}

RequestHandler* Registry::GetRequestHandler(const RequestType& type){
    auto it = request_handlers_map.find(type);
    if(it == request_handlers_map.end()){
        return nullptr;
    }
    return it->second();
}
