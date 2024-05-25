#include "session.h"
#include "request_dispatcher.h"
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include <boost/beast/http.hpp>
#include <vector>
#include <memory>
#include "logging_buffer.h"

namespace http = boost::beast::http;
session::session(boost::asio::io_service& io_service, LoggingBuffer* logging_buffer)
    : socket_(io_service), logging_buffer_(logging_buffer)
{
}
boost::asio::ip::tcp::socket& session::socket() {
    return socket_;
}
bool session::start() {
    http::async_read(socket_, 
        buff_, 
        request_, 
        boost::bind(&session::handle_read, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    return true; // Asynchronous operation setup was successful
    
}
bool session::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    
    if (!error) {
        response_ = RequestDispatcher::dispatch_request(request_, socket_, logging_buffer_);
        http::async_write(socket_,
            response_,
            boost::bind(&session::handle_write, shared_from_this(),
            boost::asio::placeholders::error));

        return true;

    } else {
        return false;
    }
}
bool session::handle_write(const boost::system::error_code& error) {
    boost::system::error_code ec;
    logging_buffer_->addToBuffer(INFO, "Sending response to " + socket_.remote_endpoint(ec).address().to_string());
    if (ec){
        logging_buffer_->addToBuffer(ERROR, "handle_write: Transport endpoint is not connected");
    }
    if (!error) {
        http::async_read(socket_, 
            buff_, 
            request_, 
            boost::bind(&session::handle_read, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
        
        return true;
    } else {
        return false;
    }
}
