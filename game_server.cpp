#include "game_server.h"
#include "fixed_messages.h"

game_server::game_server(boost::asio::io_service& io_service, int port)
	: my_tcp_server(io_service, port, this)
{
	init_fixed_msg_arg_descriptions();
}

void game_server::init_fixed_msg_arg_descriptions()
{
	// Client
	MAP_MSG_ARGS(msg_args_map, CMSG_HELLO, CMSG_HELLO_ARGS);
	MAP_MSG_ARGS(msg_args_map, CMSG_TICK, CMSG_TICK_ARGS);
}

bool game_server::call_network_interface(tcp_connection* originator, int msgID, arg_stream& args)
{
	// call function from message passed over network
	if (msgID == CMSG_HELLO)
		recv_client_hello(originator, args.get_string(), args.get_string());
	else if (msgID == CMSG_TICK)
		recv_client_tick(originator);
	else
		return false;

	return true;
}

void game_server::recv_client_hello(tcp_connection* originator, std::string protocol_name, std::string protocol_version)
{
	std::cout << "received CMSG_HELLO: protocol_name(" << protocol_name << "), protocol_version(" << protocol_version << ")" << std::endl;
	//arg_stream response;
	//originator->queue_write(response);
}

void game_server::recv_client_tick(tcp_connection* originator)
{
}
