#include "tcp_connection.h"

#include <iostream>

// utility functions

std::string tcp_connection::make_daytime_string()
{
	using namespace std;
	time_t now = time(0);
	return ctime(&now);
}

// class functions

tcp_connection::tcp_connection(boost::asio::io_service& io_service, tcp_server *server)
	: socket_(io_service),
	  server_(server)
{
}

tcp::socket& tcp_connection::socket()
{
	return socket_;
}

void tcp_connection::start()
{
	std::cout << "started tcp_connection" << std::endl;
	message_ = make_daytime_string();

	boost::asio::async_write(socket_, boost::asio::buffer(message_),
		boost::bind(&tcp_connection::handle_write, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void tcp_connection::handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/)
{
	server_->connection_closed(this);
}
