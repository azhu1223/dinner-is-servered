#include "session.h"
#include "request_dispatcher.h"
#include "request_handler.h"
#include "not_found_handler.h"
#include "echo_handler.h"
#include "static_handler.h"
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include <boost/beast/http.hpp>
#include <vector>
#include "registry.h"

namespace http = boost::beast::http;
session::session(boost::asio::io_service& io_service)
    : socket_(io_service)
{
}
boost::asio::ip::tcp::socket& session::socket() {
    return socket_;
}
bool session::start() {
    http::async_read(socket_, 
        buff_, 
        request_, 
        boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    return true; // Asynchronous operation setup was successful
    
}
bool session::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    
    if (!error) {

        //Determine the request type and get request object
        RequestType request_type = RequestDispatcher::getRequestType(request_);
        boost::system::error_code ec;

        BOOST_LOG_TRIVIAL(info) << "Request from " << socket_.remote_endpoint(ec).address() 
            << " for target " << request_.target();
        if (ec){
            BOOST_LOG_TRIVIAL(error) << "handle_read: Transport endpoint is not connected";
        }

        //Deploy to request handler
        RequestHandler* rh =  Registry::GetRequestHandler(request_type);
        response_ = rh->handle_request(request_);
        delete rh;

        http::async_write(socket_,
            response_,
            boost::bind(&session::handle_write, this,
            boost::asio::placeholders::error));

        return true;

    } else {
        return false;
    }
}
bool session::handle_write(const boost::system::error_code& error) {
    boost::system::error_code ec;
    BOOST_LOG_TRIVIAL(info) << "Sending response to " << socket_.remote_endpoint(ec).address();
    if (ec){
        BOOST_LOG_TRIVIAL(error) << "handle_write: Transport endpoint is not connected";
    }
    if (!error) {
        http::async_read(socket_, 
            buff_, 
            request_, 
            boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
        
        return true;
    } else {
        return false;
    }
}
