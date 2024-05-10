#ifndef STATIC_H
#define STATIC_H

#include "request_handler.h"
#include "utils.h"
#include <vector>


class EchoHandler : public RequestHandler {
    public:
        EchoHandler(short bytes_transferred, const char data[], ServerPaths server_paths);
        virtual std::vector<char> create_response();
        
};

#endif