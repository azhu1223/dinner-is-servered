#include "session.h"
#include "request_parser.h"
#include "response_handler.h"
#include "not_found_handler.h"
#include "echo_handler.h"
#include "static_handler.h"
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include <vector>

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
    
    if (!error) {
        RequestParser parser = RequestParser(bytes_transferred, data_, server_paths_);
        boost::system::error_code ec;
        BOOST_LOG_TRIVIAL(info) << "Request from " << socket_.remote_endpoint(ec).address() 
            << " for target " << parser.getTarget();
        if (ec){
            BOOST_LOG_TRIVIAL(error) << "handle_read: Transport endpoint is not connected";
        }

        RequestType request_type = parser.getRequestType();
        ResponseHandler* rh;
        if (request_type == Static) {
            rh = new StaticHandler(bytes_transferred, data_, server_paths_, parser.getFilePath());
        }
        else if (request_type == Echo) {
            rh = new EchoHandler(bytes_transferred, data_, server_paths_);
        }else{
            rh = new NotFoundHandler(bytes_transferred, data_, server_paths_);
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

    boost::system::error_code ec;
    BOOST_LOG_TRIVIAL(info) << "Sending response to " << socket_.remote_endpoint(ec).address();
    if (ec){
        BOOST_LOG_TRIVIAL(error) << "handle_write: Transport endpoint is not connected";
    }

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

