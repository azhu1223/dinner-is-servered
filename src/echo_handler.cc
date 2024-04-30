#include "echo_handler.h"

EchoHandler::EchoHandler(short bytes_transferred, const char data[], ServerPaths server_paths) : ResponseHandler(bytes_transferred,data,server_paths) {}