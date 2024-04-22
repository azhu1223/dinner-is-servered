#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <vector>

std::vector<char> create_response(short bytes_transferred, const char data[]);

#endif