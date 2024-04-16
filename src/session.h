#ifndef SESSION_H
#define SESSION_H
#include <string>
#include <vector>
#include <boost/asio.hpp>

class session {
public:
    session(boost::asio::io_service& io_service);
    void start();
    boost::asio::ip::tcp::socket& socket();

private:
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error);
    std::vector<char> create_response(short bytes_transferred);

    boost::asio::ip::tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};
#endif // SESSION_H