#ifndef STATIC_H
#define STATIC_H
#include "response_handler.h"

class EchoHandler : public ResponseHandler {
    public:
        EchoHandler(short bytes_transferred, const char data[], ServerPaths server_paths);
        virtual std::vector<char> create_response();
        
};

#endif