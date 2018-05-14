#include "message_reader.h"
#include "fixed_messages.h"

message_reader::message_reader()
	: next_type(ARG_UINT16) // first arg is always msg ID
{
	init_msg_args();
}

void message_reader::process()
{
}

void message_reader::init_msg_args()
{
	// Client
	MAP_MSG_ARGS(msg_args, CMSG_HELLO, CMSG_HELLO_ARGS);
	MAP_MSG_ARGS(msg_args, CMSG_TICK, CMSG_TICK_ARGS);
}
