#include "message_reader.h"
#include "fixed_messages.h"

message_reader::message_reader(bool is_internal = false)
	: cur_msg_args(NULL),
	  cur_msg_args_index(0),
	  _is_internal(is_internal)
{
	spillover_buffer.reserve(MAX_SAFE_ARG_SIZE_BYTES);

	init_msg_args();
}

bool message_reader::process(char* data, size_t len)
{
}

void message_reader::init_msg_args()
{
	// Client
	MAP_MSG_ARGS(msg_args, CMSG_HELLO, CMSG_HELLO_ARGS);
	MAP_MSG_ARGS(msg_args, CMSG_TICK, CMSG_TICK_ARGS);
}
