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
	virtual bool call_network_interface(tcp_connection* originator, int msgID, arg_stream& args)=0;
	virtual std::vector<ARG_TYPE>* get_msg_args_by_id(MSG_ID msgID)=0;
};

class message_reader
{
public:
	message_reader(message_handler* handler_ptr, tcp_connection* tcp_connection_ptr);

	// returns false if an invalid msg id is given or spillover_buffer is full.
	bool process(char* data, size_t len);
private:
	// init message arguments so we know how long each message will be in bytes
	static void init_msg_args();

	message_handler* handler_ptr;

	char buffer[MAX_SAFE_ARG_SIZE_BYTES];
	static const int buffer_capacity = MAX_SAFE_ARG_SIZE_BYTES;
	bool write_to_buffer(char* data, size_t len);
	bool read_from_buffer(char* dest, size_t len);
	bool peek_from_buffer(char* dest, size_t len)
	{
		size_t old = rpos;
		bool ret = read_from_buffer(dest, len);
		rpos = old;
		return ret;
	}
	size_t get_buffer_read_left() { return wpos-rpos; }
	size_t get_buffer_write_left() { return buffer_capacity - get_buffer_read_left(); }
	bool buffer_can_read(size_t len) { return get_buffer_read_left() >= len; }
	size_t find_in_buffer(char c);
	size_t rpos;
	size_t wpos;

	enum class READ_RESULT { SUCCESS, ERROR, NEED_READ_MORE };
	READ_RESULT read_cur_type_to_args_stream();

	arg_stream cur_arg_stream;

	std::vector< std::vector<MSG_ID> > msg_args_stack;
	int cur_msg_args_index;

	MSG_ID cur_msg_id;

	message_handler* handler_ptr;
};

#endif // MESSAGEREADER_H
