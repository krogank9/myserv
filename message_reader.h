#ifndef MESSAGEREADER_H
#define MESSAGEREADER_H

#include <map>
#include <vector>
#include <stdint.h>
#include <boost/array.hpp>
#include "arg_stream.h"

class message_reader
{
public:
	message_reader();

	void process();

	boost::array<uint8_t, 1024>& get_buffer() { return buffer; }
private:
	boost::array<uint8_t, 1024> buffer;

	int next_type;

	// init message arguments so we know how long each message will be in bytes
	void init_msg_args();

	std::map< int, std::vector<int> > msg_args;
};

#endif // MESSAGEREADER_H
