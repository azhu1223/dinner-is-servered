#ifndef CRUD_HANDLER_H
#define CRUD_HANDLER_H

#include "request_handler.h"
#include "utils.h"
#include <vector>
#include <unordered_set>
#include "crud_file_manager.h"

namespace http = boost::beast::http;

class CrudHandler : public RequestHandler {
    public:
        CrudHandler(CrudFileManager manager);
        virtual http::response<http::vector_body<char>> handle_request(const http::request<http::vector_body<char>>& req);
        static std::string generateEntityID(std::unordered_set<std::string> used_ids);
    private:
        CrudFileManager file_manager;
};

class CrudHandlerFactory{
    public:
        static RequestHandler* create();
};

#endif