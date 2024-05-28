#ifndef SERVER_H
#define SERVER_H

#include "session.h"
#include "utils.h"
#include "logging_buffer.h"
#include <boost/asio.hpp>
#include <thread>
#include <memory>
#include <atomic>

class server {
public:
    server(boost::asio::io_service& io_service, LoggingBuffer* logging_buffer, int threadpool_size, short port);
    void run();
    bool start_accept();
    static void launch_session(std::shared_ptr<session> new_session);
private:
    void handle_accept(std::shared_ptr<session> new_session, const boost::system::error_code& error);
    void handle_logging();
    bool kill_server();
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
    LoggingBuffer* logging_buffer_;
    std::thread logging_thread_;
    std::atomic<bool> program_terminated_;
    int threadpool_size_;
    std::vector<std::shared_ptr<std::thread>> threadpool_;
    boost::asio::signal_set signals_;
};
#endif // SERVER_H