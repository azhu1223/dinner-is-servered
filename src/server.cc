#include "server.h"
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include "logger.h"

server::server(boost::asio::io_service& io_service, short port)
    : io_service_(io_service), acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
    BOOST_LOG_TRIVIAL(info) << "Starting server on port " << port;
    start_accept();
}

bool server::start_accept() {
    BOOST_LOG_TRIVIAL(info) << "Server now accepting requests";
    session* new_session = new session(io_service_);
    if (new_session == NULL) {
        return false;
    }
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
    return true;
}

void server::handle_accept(session* new_session, const boost::system::error_code& err) {
    
    if (!err) {
        new_session->start();
    } else {
        BOOST_LOG_TRIVIAL(error) << "Error accepting sesssion: " << err;
        delete new_session;
    }
    start_accept();
}

void server::run() {
    BOOST_LOG_TRIVIAL(info) << "Running the server";
    io_service_.run();
}
