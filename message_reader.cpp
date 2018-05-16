#include "message_reader.h"
#include "fixed_messages.h"

message_reader::message_reader(message_handler* handler_ptr, tcp_connection* tcp_connection_ptr)
	: cur_msg_args_ptr(NULL),
	  cur_msg_args_index(0),
	  handler_ptr(handler_ptr),
	  parent_tcp_connection_ptr(tcp_connection_ptr)
{
	spillover_buffer.reserve(MAX_SAFE_ARG_SIZE_BYTES);
}

bool message_reader::process(char* data, size_t len)
{
}
