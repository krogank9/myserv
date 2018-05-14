class tcp_connection;

#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "tcp_server.h"

using boost::asio::ip::tcp;

class tcp_connection
{
public:
	tcp_connection(boost::asio::io_service& io_service, tcp_server *server);
	tcp::socket& socket();
	void start();

private:
	std::string make_daytime_string();

	void handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/);

	tcp::socket socket_;
	std::string message_;

	tcp_server *server_;
};
#endif // TCP_CONNECTION_H
