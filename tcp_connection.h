#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "message_reader.h"

class tcp_server;
class tcp_connection;
class message_handler;

using boost::asio::ip::tcp;

class tcp_connection_manager {
public:
	virtual void connection_closed(tcp_connection*)=0;
};

class tcp_connection
{
public:
	tcp_connection(boost::asio::io_service& io_service, message_handler* message_handler_ptr,
				   bool peer_is_server_=false, tcp_connection_manager* manager=NULL);
	tcp::socket& socket();
	void start();

	void queue_write(arg_stream* msg);

	bool peer_is_server() { return peer_is_server_; }
	bool peer_is_client() { return !peer_is_server_; }
private:
	std::string make_daytime_string();
	void handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/);

	bool peer_is_server_;

	message_reader message_reader_;

	tcp::socket socket_;
	std::string message;

	tcp_connection_manager* manager_ptr;
};
#endif // TCP_CONNECTION_H
