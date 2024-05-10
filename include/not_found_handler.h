#ifndef NOT_FOUND_HANDLER_H
#define NOT_FOUND_HANDLER_H

#include "request_handler.h"
#include "utils.h"
#include <vector>


class NotFoundHandler : public RequestHandler {
    public:
        NotFoundHandler(short bytes_transferred, const char data[], ServerPaths server_paths);
        virtual std::vector<char> create_response();
        
};

#endif