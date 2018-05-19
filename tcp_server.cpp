#include "tcp_server.h"
#include "message_reader.h"
#include <iostream>

tcp_server::tcp_server(boost::asio::io_service& io_service, int port, game_server* game_server_ptr)
	: acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
	  parent_game_server_ptr(game_server_ptr)
{
		start_accept();
}

tcp_server::~tcp_server()
{
	std::set<tcp_connection*>::iterator it = client_list.begin();
	for (;it != client_list.end(); it++)
		delete *it;
}

void tcp_server::start_accept()
{
	tcp_connection *new_connection = new tcp_connection(acceptor_.get_io_service(),
														(message_handler*)parent_game_server_ptr,
														false, this);
	client_list.insert(new_connection);

	acceptor_.async_accept(new_connection->socket(),
		boost::bind(&tcp_server::handle_accept, this, new_connection,
			boost::asio::placeholders::error));
}

void tcp_server::handle_accept(tcp_connection *new_connection, const boost::system::error_code& error)
{
	if (!error)
	{
		std::cout << "starting new_connection" << std::endl;
		new_connection->start();
	}

	start_accept();
}

void tcp_server::connection_closed(tcp_connection *c)
{
	std::cout << "connection closed" << std::endl;
	if (client_list.find(c) != client_list.end())
	{
		client_list.erase(c);
		delete c;
	}
}
