#include <vector>
#include <map>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "tcp_connection.h"
#include "fixed_messages.h"
#include "arg_stream.h"

using boost::asio::ip::tcp;

using namespace std;

class client_test_handler : public message_handler
{
public:
	client_test_handler()
	{
		init_msg_args();
		game_update();
	}

	bool call_network_interface(tcp_connection* originator, int msgID, arg_stream& args)
	{
		if (msgID == CMSG_TICK)
			std::cout << "received tick from server" << std::endl;
	}

	std::vector<ARG_TYPE>* get_msg_args_by_id(MSG_ID msgID)
	{
		return &(msg_args_map[msgID]);
	}

	void game_update()
	{
	}

private:
	void init_msg_args()
	{
		MAP_MSG_ARGS(msg_args_map, CMSG_HELLO, CMSG_HELLO_ARGS);
		MAP_MSG_ARGS(msg_args_map, CMSG_TICK, CMSG_TICK_ARGS);
	}

	//boost::asio::deadline_timer update_timer;
	static const int update_period_ms = 10;
	std::map< MSG_ID, std::vector<ARG_TYPE> > msg_args_map;
};

int main()
{
	try
	{
		boost::asio::io_service io_service;

		tcp::resolver resolver(io_service);
		tcp::resolver::query query("127.0.0.1", "1025");
		tcp::resolver::iterator endpoint_it = resolver.resolve(query);

		client_test_handler msg_handler;

		tcp_connection connection(io_service, &msg_handler, true);
		boost::asio::connect(connection.socket(), endpoint_it);

		connection.start();

		boost::shared_ptr<arg_stream> hello_msg = boost::make_shared<arg_stream>();
		hello_msg->put_msg_id(CMSG_HELLO);
		hello_msg->put_string("myserv");
		hello_msg->put_string("1.0");
		connection.queue_write(hello_msg);

		std::cout << "running io_service" << std::endl;
		io_service.run();
		std::cout << "io_service done" << std::endl;
	}
	catch (std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
	}

	return 0;
}
