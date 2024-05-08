#ifndef NOT_FOUND_HANDLER_H
#define NOT_FOUND_HANDLER_H

#include "response_handler.h"
#include "utils.h"
#include <vector>


class NotFoundHandler : public ResponseHandler {
    public:
        NotFoundHandler(short bytes_transferred, const char data[], ServerPaths server_paths);
        virtual std::vector<char> create_response();
        
};

#endif