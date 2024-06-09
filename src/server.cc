#include "server.h"
#include "utils.h"
#include "logging_buffer.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include <memory>
#include <vector>
#include <atomic>
#include <thread>

server::server(boost::asio::io_service& io_service, LoggingBuffer* logging_buffer, int threadpool_size, short port): io_service_(io_service),
    acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
    program_terminated_(false),
    logging_buffer_(logging_buffer),
    logging_thread_(&server::handle_logging, this),
    threadpool_size_(threadpool_size),
    signals_(io_service_)
{
    signals_.add(SIGINT);
    signals_.add(SIGTERM);

    signals_.async_wait(boost::bind(&server::kill_server, this));
    logging_buffer->addToBuffer(INFO, "Starting server on port " + port);

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
    logging_buffer_->addToBuffer(INFO, "Running the server");

    logging_buffer_->addToBuffer(INFO, "Creating threadpool");
    for(int i = 0; i < threadpool_size_; i++) {
        std::shared_ptr<std::thread> thread(new std::thread(
            boost::bind(&boost::asio::io_service::run, &io_service_)));
        threadpool_.push_back(thread);
    }

    logging_buffer_->addToBuffer(INFO, "Joining threadpool threads");
    for (auto thread : threadpool_) {
        thread->join();
    }
}

void server::handle_logging() {
    while (!program_terminated_.load() || !logging_buffer_->empty()) {
        logging_buffer_->writeToLog();
    }
}

bool server::kill_server() {
    logging_buffer_->addToBuffer(INFO, "Recieved interupt signal. Terminating");

    io_service_.stop();
    program_terminated_.store(true);
    logging_thread_.join();
}