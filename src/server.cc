#include "server.h"
#include "utils.h"
#include "logging_buffer.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>

server::server(boost::asio::io_service& io_service, LoggingBuffer* logging_buffer, short port): io_service_(io_service),
    acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
    logging_buffer_(logging_buffer),
    logging_thread_(&server::handle_logging, this)
{
    BOOST_LOG_TRIVIAL(info) << "Starting server on port " << port;
    start_accept();
}

bool server::start_accept() {
    BOOST_LOG_TRIVIAL(info) << "Server now accepting requests.";
    std::shared_ptr<session> new_session = std::make_shared<session>(io_service_, logging_buffer_);
    if (new_session == NULL) {
        return false;
    }
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
    return true;
}

void server::handle_accept(std::shared_ptr<session> new_session, const boost::system::error_code& err) {
    
    if (!err) {
        new_session->start();
    } else {
        BOOST_LOG_TRIVIAL(error) << "Error accepting sesssion: " << err;
    }
    start_accept();
}

void server::run() {
    BOOST_LOG_TRIVIAL(info) << "Running the server";
    io_service_.run();
}

void server::handle_logging() {
    while (true) {
        logging_buffer_->writeToLog();
    }
}