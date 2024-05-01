#include "session.h"
#include <string>
#include <vector>
#include <boost/bind.hpp>
#include "response_handler.h"
#include <boost/log/trivial.hpp>
#include "echo_handler.h"
#include "static_handler.h"
#include "request_parser.h"
#include <iostream>

session::session(boost::asio::io_service& io_service, ServerPaths server_paths)
    : socket_(io_service), server_paths_(server_paths) 
{

}

boost::asio::ip::tcp::socket& session::socket() {
    return socket_;
}

bool session::start() {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&session::handle_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        return true; // Asynchronous operation setup was successful
}

bool session::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {

    BOOST_LOG_TRIVIAL(info) << "Request from " << socket_.remote_endpoint().address();
    
    
    
    if (!error) {
        RequestParser parser = RequestParser(bytes_transferred, data_, server_paths_);
        BOOST_LOG_TRIVIAL(info) << "Is static request? " << parser.isRequestStatic();
        BOOST_LOG_TRIVIAL(info) << "Is echo request? " << parser.isRequestEcho();
        ResponseHandler* rh;
        if (parser.isRequestStatic()) {
            rh = new StaticHandler(bytes_transferred, data_, server_paths_, parser.getFilePath());
        }
        else if (parser.isRequestEcho()) {
            rh = new EchoHandler(bytes_transferred, data_, server_paths_);
        }

        std::vector<char> response = rh->create_response();
        delete rh;

        boost::asio::async_write(socket_,
            boost::asio::buffer(response.data(), response.size()),
            boost::bind(&session::handle_write, this,
              boost::asio::placeholders::error));
        return true;
    } else {
        return false;
    }
}

bool session::handle_write(const boost::system::error_code& error) {
    BOOST_LOG_TRIVIAL(info) << "Sending response to " << socket_.remote_endpoint().address();

    if (!error) {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&session::handle_read, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
        return true;
    } else {
        // delete this;
        return false;
    }
}

