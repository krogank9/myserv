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
	  manager_ptr(manager_ptr),
	  timeout_timer(io_service),
	  already_notified_close(false)
{
}

tcp::socket& tcp_connection::socket()
{
	return socket_;
}

void tcp_connection::start()
{
	message = make_daytime_string();

	timeout_timer.expires_from_now(boost::posix_time::seconds(read_timeout_seconds));
	timeout_timer.async_wait(boost::bind(&tcp_connection::handle_timeout, this,
										 boost::asio::placeholders::error));

	socket_.async_read_some(boost::asio::buffer(read_buffer_bytes, read_buffer_size),
							boost::bind(&tcp_connection::handle_read, this,
										boost::asio::placeholders::error,
										boost::asio::placeholders::bytes_transferred));
}

void tcp_connection::queue_write(boost::shared_ptr<arg_stream> msg)
{
	boost::asio::async_write(socket_, boost::asio::buffer(msg->get_buffer(), msg->length()),
							 boost::bind(&tcp_connection::handle_write, this,
										 boost::asio::placeholders::error,
										 boost::asio::placeholders::bytes_transferred,
										 msg));
}

void tcp_connection::close_connection()
{
	socket_.close();
	if (manager_ptr != NULL && !already_notified_close)
	{
		already_notified_close = true;
		manager_ptr->connection_closed(this);
	}
}

void tcp_connection::queue_close_connection()
{
	socket_.get_io_service().post(boost::bind(&tcp_connection::close_connection, this));
}

void tcp_connection::handle_timeout(const boost::system::error_code& error)
{
	if (error == boost::asio::error::operation_aborted)
		return;
	std::cout << "received timeout" << std::endl;
	close_connection();
}

void tcp_connection::handle_write(const boost::system::error_code& error, size_t /*bytes_transferred*/,
								  boost::shared_ptr<arg_stream> /*ensure buffer stays in memory till write over*/)
{
	if (error != 0)
		close_connection();
}

void tcp_connection::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (error != 0)
		close_connection();
	if (!socket_.is_open())
		return;

	timeout_timer.expires_from_now(boost::posix_time::seconds(read_timeout_seconds));
	timeout_timer.async_wait(boost::bind(&tcp_connection::handle_timeout, this,
										 boost::asio::placeholders::error));

	message_reader_.process(read_buffer_bytes, bytes_transferred);
	socket_.async_read_some(boost::asio::buffer(read_buffer_bytes, read_buffer_size),
							boost::bind(&tcp_connection::handle_read, this,
								boost::asio::placeholders::error,
								boost::asio::placeholders::bytes_transferred));
}
