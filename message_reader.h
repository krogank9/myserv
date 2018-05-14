#ifndef MESSAGEREADER_H
#define MESSAGEREADER_H

#include <map>
#include <vector>
#include <stdint.h>
#include <boost/array.hpp>
#include "arg_stream.h"
#include "fixed_messages.h"

class message_reader
{
public:
	message_reader(bool is_internal);

	// returns false if an invalid msg id is given or spillover_buffer is full.
	bool process(char* data, size_t len);
private:
	// whether this is an internal connection to another server.
	// if so, we can safely use an unlimited spillover for things like serializing entities.
	bool _is_internal;

	// spillover for when an arg gets segmented between process() calls.
	std::vector<char> spillover_buffer;

	// init message arguments so we know how long each message will be in bytes
	void init_msg_args();

	arg_stream cur_arg_stream;

	std::vector<int> *cur_msg_args;
	int cur_msg_args_index;

	std::map< int, std::vector<int> > msg_args;
};

#endif // MESSAGEREADER_H
