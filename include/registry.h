#ifndef REGISTRY_H
#define REGISTRY_H

#include <map>
#include <string>
#include "request_handler.h"
#include "request_parser.h"

typedef RequestHandler* (*RequestHandlerFactory)();

class Registry{
    public:
        static void RegisterRequestHandler(const RequestType& type, RequestHandlerFactory factory);
        static RequestHandler* GetRequestHandler(const RequestType& type);
        ~Registry();
    private:
        static std::map<RequestType, RequestHandlerFactory> request_handlers_map;

};

#endif // REGISTRY_H