#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "message_reader.h"

class tcp_server;
class message_handler;

using boost::asio::ip::tcp;

class tcp_connection
{
public:
	tcp_connection(boost::asio::io_service& io_service, message_handler* message_handler_ptr, tcp_server *server_ptr=NULL);
	tcp::socket& socket();
	void start();

	bool is_server() { return parent_server_ptr != NULL; }
private:
	std::string make_daytime_string();
	void handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/);

	message_reader message_reader_;

	tcp::socket socket_;
	std::string message;

	tcp_server *parent_server_ptr;
};
#endif // TCP_CONNECTION_H
