#ifndef SESSION_H
#define SESSION_H
#include <string>
#include <vector>
#include <boost/asio.hpp>

class session {
public:
    session(boost::asio::io_service& io_service);
    bool start();
    boost::asio::ip::tcp::socket& socket();
    bool handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    bool handle_write(const boost::system::error_code& error);

private:

    boost::asio::ip::tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    boost::asio::streambuf request_;
};
#endif // SESSION_H