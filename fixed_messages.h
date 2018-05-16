#ifndef FIXEDMESSAGES_H
#define FIXEDMESSAGES_H

#include <stdint.h>
#include <map>
#include <vector>

#define MAX_SAFE_ARG_SIZE_BYTES (1024*5) // 5kb. Note: This many bytes allocated per client

// Message arg types

#define ARG_UINT8 0
#define ARG_UINT16 1
#define ARG_UINT32 2
#define ARG_UINT64 3

#define ARG_INT8 4
#define ARG_INT16 5
#define ARG_INT32 6
#define ARG_INT64 7

#define ARG_FLOAT 8 // 32 bit IEEE floating point
#define ARG_DOUBLE 9 // 64 bit IEEE floating point

#define ARG_STRING 10 // null terminated ascii string
#define ARG_ARRAY 11 // uint64_t (size) prefixed, list of elements each prefixed by uint8_t type
#define ARG_DICT 12 // uint64_t (size) prefixed, list of (string, element prefixed by uint8_t type) pairs

#define ARG_BLOB 13 // uint64_t (size) prefixed, arbitrary data blob

// Client <-> Server messages

#define CMSG_HELLO 0
#define CMSG_HELLO_ARGS {ARG_STRING /*protocol name*/, ARG_STRING /*version code*/}
#define CMSG_TICK 1
#define CMSG_TICK_ARGS {}

// Server <-> Master server messages

#define SMSG_HELLO 0
#define SMSG_HELLO_ARGS {ARG_STRING /*protocol name*/, ARG_STRING /*version code*/}
#define SMSG_TICK 1
#define SMSG_TICK_ARGS {}

// set map id key to a vector from _ARGS from above
#define MAP_MSG_ARGS(map, id, args)\
{\
	int args_arr[] = args;\
	if (sizeof(args_arr) > 0)\
		map[id] = std::vector<int>( args_arr, args_arr + sizeof(args_arr)/sizeof(args_arr[0]) );\
	else\
		map[id] = std::vector<int>();\
}

#endif // FIXEDMESSAGES_H

