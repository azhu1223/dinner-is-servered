#include "server.h"
#include <boost/bind.hpp>
server::server(boost::asio::io_service& io_service, short port)
    : io_service_(io_service), acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
    start_accept();
}

bool server::start_accept() {
    session* new_session = new session(io_service_);
    if (new_session == NULL) {
        return false;
    }
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));

    return true;
}

void server::handle_accept(session* new_session, const boost::system::error_code& error) {
    if (!error) {
        new_session->start();
    } else {
        delete new_session;
    }
    start_accept();
}

void server::run() {
    io_service_.run();
}
