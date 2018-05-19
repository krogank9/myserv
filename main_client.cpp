#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "tcp_connection.h"
#include "fixed_messages.h"
#include "arg_stream.h"

using boost::asio::ip::tcp;

using namespace std;

int main()
{
	try
	{
		boost::asio::io_service io_service;

		tcp::resolver resolver(io_service);
		tcp::resolver::query query("127.0.0.1", "1025");
		tcp::resolver::iterator endpoint_it = resolver.resolve(query);

		tcp_connection connection(io_service, NULL, true);
		boost::asio::connect(connection.socket(), endpoint_it);

		arg_stream hello_msg;
		hello_msg.put_msg_id(CMSG_HELLO);
		hello_msg.put_string("hi");
		hello_msg.put_string("hi");
		connection.queue_write(hello_msg);
		io_service.run();
	}
	catch (std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
	}

	return 0;
}
