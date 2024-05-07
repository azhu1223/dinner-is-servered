#ifndef SERVER_H
#define SERVER_H

#include "session.h"
#include "utils.h"
#include <boost/asio.hpp>


class server {
public:
    server(boost::asio::io_service& io_service, short port, ServerPaths server_paths);
    void run();
    bool start_accept();


private:
    void handle_accept(session* new_session, const boost::system::error_code& error);
    ServerPaths server_paths_;
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
};
#endif // SERVER_H