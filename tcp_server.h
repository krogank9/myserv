#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <set>

#include "tcp_connection.h"
class game_server;

using boost::asio::ip::tcp;

class tcp_server
{
public:
	tcp_server(boost::asio::io_service& io_service, int port, game_server* game_server_ptr);

	void connection_closed(tcp_connection *c);
private:
	void start_accept();
	void handle_accept(tcp_connection *new_connection, const boost::system::error_code& error);

	tcp::acceptor acceptor_;
	std::set<tcp_connection*> client_list;

	game_server* parent_game_server_ptr;
};

#endif // TCP_SERVER_H
