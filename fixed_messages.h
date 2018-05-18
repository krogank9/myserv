#ifndef FIXEDMESSAGES_H
#define FIXEDMESSAGES_H

#include <stdint.h>
#include <map>
#include <vector>

#define MAX_SAFE_ARG_SIZE_BYTES (1024*5) // 5kb. Note: This many bytes allocated per client

// Message arg types

typedef uint16_t MSG_ID;
typedef uint8_t ARG_TYPE;

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
#define ARG_BLOB 11 // uint16_t (size) prefixed, arbitrary data blob

#define ARG_ARRAY 13 // uint16_t (size) prefixed, list of elements each prefixed by uint8_t type
#define ARG_DICT 12 // uint16_t (size) prefixed, list of (ARG_PROP, ARG_PROP) pairs

#define ARG_PROP 14 // uint8_t (ARG_TYPE) prefixed followed by corresponding ARG_*

// fixed type containers to save bandwidth

#define ARG_ARRAY_UINT8 15
#define ARG_ARRAY_UINT16 16
#define ARG_ARRAY_UINT32 17
#define ARG_ARRAY_UINT64 18
#define ARG_ARRAY_INT8 19
#define ARG_ARRAY_INT16 20
#define ARG_ARRAY_INT32 21
#define ARG_ARRAY_INT64 22
#define ARG_ARRAY_FLOAT 23
#define ARG_ARRAY_DOUBLE 24
#define ARG_ARRAY_STRING 25

#define ARG_DICT_STRING 27
#define ARG_DICT_STRING_UINT8 28
#define ARG_DICT_STRING_UINT16 29
#define ARG_DICT_STRING_UINT32 30
#define ARG_DICT_STRING_UINT64 31
#define ARG_DICT_STRING_INT8 32
#define ARG_DICT_STRING_INT16 33
#define ARG_DICT_STRING_INT32 34
#define ARG_DICT_STRING_INT64 35
#define ARG_DICT_STRING_FLOAT 36
#define ARG_DICT_STRING_DOUBLE 37
#define ARG_DICT_STRING_STRING 38

size_t ARG_SIZEOF(ARG_TYPE arg)
{
	// UINT
	if (arg == ARG_UINT8)
		return sizeof(uint8_t);
	if (arg == ARG_UINT16)
		return sizeof(uint16_t);
	if (arg == ARG_UINT32)
		return sizeof(uint32_t);
	if (arg == ARG_UINT64)
		return sizeof(uint64_t);
	// INT
	if (arg == ARG_INT8)
		return sizeof(int8_t);
	if (arg == ARG_INT16)
		return sizeof(int16_t);
	if (arg == ARG_INT32)
		return sizeof(int32_t);
	if (arg == ARG_INT64)
		return sizeof(int64_t);
	// FLOAT/DOUBLE
	if (arg == ARG_FLOAT)
		return sizeof(float);
	if (arg == ARG_DOUBLE)
		return sizeof(double);
	return 0;
}

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

