#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "tcp_server.h"
#include "tcp_connection.h"
#include "arg_stream.h"
#include "message_reader.h"

class game_server : public message_handler
{
public:
	game_server(boost::asio::io_service& io_service, int port);

	virtual bool call_network_interface(tcp_connection* source_tcp_connection, int msgID, arg_stream& args);

	std::vector<ARG_TYPE> get_msg_args_by_id(MSG_ID msgID) { return msg_args_map[msgID]; }

private:
	// arg descriptions used by all message readers to validate data and get each message length
	virtual void init_fixed_msg_arg_descriptions();

	// Network interfaces
	void recv_client_hello(tcp_connection* originator, std::string protocol_name, std::string protocol_version);
	void recv_client_tick(tcp_connection* originator);

	tcp_server my_tcp_server;

	std::map< MSG_ID, std::vector<ARG_TYPE> > msg_args_map;
};

#endif // GAME_SERVER_H
