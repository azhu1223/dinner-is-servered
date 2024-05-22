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
        CrudHandler(std::shared_ptr<CrudFileManager> manager);
        ~CrudHandler();
        virtual http::response<http::vector_body<char>> handle_request(const http::request<http::vector_body<char>>& req);
    private:
        http::response<http::vector_body<char>> handle_post(CrudPath path, const http::request<http::vector_body<char>>& req);
        http::response<http::vector_body<char>> handle_list(CrudPath path, const http::request<http::vector_body<char>>& req);
        http::response<http::vector_body<char>> handle_get(CrudPath path, const http::request<http::vector_body<char>>& req);
        http::response<http::vector_body<char>> handle_put(CrudPath path, const http::request<http::vector_body<char>>& req);
        http::response<http::vector_body<char>> handle_del(CrudPath path, const http::request<http::vector_body<char>>& req);

        std::shared_ptr<CrudFileManager> file_manager;
        int max_unused_entity_id = 1; 
};

class CrudHandlerFactory{
    public:
        static RequestHandler* create();
};

#endif