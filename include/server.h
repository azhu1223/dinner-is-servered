#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include "session.h"

class server {
public:
    server(boost::asio::io_service& io_service, short port);
    void run();

private:
    void start_accept();
    void handle_accept(session* new_session, const boost::system::error_code& error);

    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
};
#endif // SERVER_H