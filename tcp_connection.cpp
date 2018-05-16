#include "tcp_connection.h"
#include "tcp_server.h"

#include <iostream>

// utility functions

std::string tcp_connection::make_daytime_string()
{
	using namespace std;
	time_t now = time(0);
	return ctime(&now);
}

// class functions

tcp_connection::tcp_connection(boost::asio::io_service& io_service, message_handler* message_handler_ptr,
							   bool peer_is_server_, tcp_connection_manager* manager_ptr)
	: socket_(io_service),
	  message_reader_(message_handler_ptr, this),
	  peer_is_server_(peer_is_server_),
	  manager_ptr(manager_ptr)
{
}

tcp::socket& tcp_connection::socket()
{
	return socket_;
}

void tcp_connection::start()
{
	std::cout << "started tcp_connection" << std::endl;
	message = make_daytime_string();

	boost::asio::async_write(socket_, boost::asio::buffer(message),
		boost::bind(&tcp_connection::handle_write, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void tcp_connection::handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/)
{
	if (manager_ptr != NULL)
		manager_ptr->connection_closed(this);
}
