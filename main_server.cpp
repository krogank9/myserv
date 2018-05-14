#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <set>

#include "tcp_server.h"

using boost::asio::ip::tcp;

int main()
{
	std::cout << "Starting server" << std::endl;
	try
	{
		boost::asio::io_service io_service;
		tcp_server server(io_service, 1025);
		io_service.run();
	}
	catch (std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
	}

	return 0;
}
