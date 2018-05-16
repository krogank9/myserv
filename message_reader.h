#ifndef MESSAGEREADER_H
#define MESSAGEREADER_H
class message_reader;

#include <map>
#include <vector>
#include <stdint.h>
#include <boost/array.hpp>
#include "arg_stream.h"
#include "fixed_messages.h"

class game_server;
class tcp_connection;

class message_handler {
	virtual void call_network_interface(tcp_connection* originator, int msgID, arg_stream& args)=0;
};

class message_reader
{
public:
	message_reader(message_handler* handler_ptr, tcp_connection* tcp_connection_ptr);

	// returns false if an invalid msg id is given or spillover_buffer is full.
	bool process(char* data, size_t len);
private:
	message_handler* handler_ptr;

	// spillover for when an arg gets segmented between process() calls.
	std::vector<char> spillover_buffer;

	// init message arguments so we know how long each message will be in bytes
	static void init_msg_args();

	arg_stream cur_arg_stream;

	std::vector<int>* cur_msg_args_ptr;
	int cur_msg_args_index;

	tcp_connection* parent_tcp_connection_ptr;
};

#endif // MESSAGEREADER_H
