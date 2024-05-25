#ifndef SERVER_H
#define SERVER_H

#include "session.h"
#include "utils.h"
#include "logging_buffer.h"
#include <boost/asio.hpp>
#include <thread>
#include <memory>

class server {
public:
    server(boost::asio::io_service& io_service, LoggingBuffer* logging_buffer, short port);
    void run();
    bool start_accept();
    static void launch_session(std::shared_ptr<session> new_session);

private:
    void handle_accept(std::shared_ptr<session> new_session, const boost::system::error_code& error);
    void handle_logging();
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
    LoggingBuffer* logging_buffer_;
    std::thread logging_thread_;
};
#endif // SERVER_H