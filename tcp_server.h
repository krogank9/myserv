#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <set>

#include "tcp_connection.h"
class game_server;
class tcp_connection_manager;

using boost::asio::ip::tcp;

// after tcp server/connection model is stable:
// TODO: generalize tcp_server and tcp_connection to udp/tcp server and connection
// the way this will work is it will support up to 2 connections per account, one fast and one slow.
// with get_fast_connection() (prioritize return udp) and get_reliable_connection() (prioritize return tcp)
// functions, and when sending replies to client, some functions (movement updating) will use get_fast_connection()
// TODO: also generalize tcp_connection to websocket/raw tcp by checking first 5 bytes against http req codes

class tcp_server : public tcp_connection_manager
{
public:
	tcp_server(boost::asio::io_service& io_service, int port, game_server* game_server_ptr);
	~tcp_server();

	void connection_closed(tcp_connection *c);
private:
	void start_accept();
	void handle_accept(tcp_connection *new_connection, const boost::system::error_code& error);

	tcp::acceptor acceptor_;
	std::set<tcp_connection*> client_list;

	game_server* parent_game_server_ptr;
};

#endif // TCP_SERVER_H
