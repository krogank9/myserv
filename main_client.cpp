#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

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

		tcp::socket socket(io_service);
		boost::asio::connect(socket, endpoint_it);

		for (;;)
		{
			boost::array<char, 128> buf;
			boost::system::error_code error;

			size_t len = socket.read_some(boost::asio::buffer(buf), error);

			if (error == boost::asio::error::eof) // Connection closed cleanly by peer
				break;
			else if (error)
				throw boost::system::system_error(error); // Some other error

			std::cout.write(buf.data(), len);
		}
	}
	catch (std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
	}

	return 0;
}
