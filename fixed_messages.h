#ifndef FIXEDMESSAGES_H
#define FIXEDMESSAGES_H

#include <stdint.h>
#include <map>
#include <vector>

#define MAX_SAFE_ARG_SIZE_BYTES (1024*5) // 5kb. Note: This many bytes allocated per client

// Message arg types

typedef uint16_t MSG_ID;
typedef uint8_t ARG_TYPE;
typedef uint16_t DICT_LEN;
typedef uint16_t ARRAY_LEN;
typedef uint16_t BLOB_LEN;

#define ARG_UINT8 0
#define ARG_BOOL 0
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
#define ARG_BLOB 11 // BLOB_LEN prefixed, arbitrary data blob

#define ARG_ARRAY 12 // ARRAY_LEN prefixed, list of elements each prefixed by uint8_t type
#define ARG_DICT 13 // DICT_LEN prefixed, list of (ARG_PROP, ARG_PROP) pairs

#define ARG_PROP 14 // ARG_TYPE prefixed, followed by corresponding ARG_*

#define ARG_VECTOR2 15
#define ARG_VECTOR3 16

// fixed type containers

#define ARG_ARRAY_UINT8 17
#define ARG_ARRAY_UINT16 18
#define ARG_ARRAY_UINT32 19
#define ARG_ARRAY_UINT64 20
#define ARG_ARRAY_INT8 21
#define ARG_ARRAY_INT16 22
#define ARG_ARRAY_INT32 23
#define ARG_ARRAY_INT64 24
#define ARG_ARRAY_FLOAT 25
#define ARG_ARRAY_DOUBLE 26
#define ARG_ARRAY_STRING 27

#define ARG_DICT_STRING 28
#define ARG_DICT_STRING_UINT8 29
#define ARG_DICT_STRING_UINT16 30
#define ARG_DICT_STRING_UINT32 31
#define ARG_DICT_STRING_UINT64 32
#define ARG_DICT_STRING_INT8 33
#define ARG_DICT_STRING_INT16 34
#define ARG_DICT_STRING_INT32 35
#define ARG_DICT_STRING_INT64 36
#define ARG_DICT_STRING_FLOAT 37
#define ARG_DICT_STRING_DOUBLE 38
#define ARG_DICT_STRING_STRING 39

// macros to get fixed type container element types

#define ARG_IS_ARRAY(arg) (arg==ARG_ARRAY\
	|| arg==ARG_ARRAY_UINT8\
	|| arg==ARG_ARRAY_UINT16\
	|| arg==ARG_ARRAY_UINT32\
	|| arg==ARG_ARRAY_UINT64\
	|| arg==ARG_ARRAY_INT8\
	|| arg==ARG_ARRAY_INT16\
	|| arg==ARG_ARRAY_INT32\
	|| arg==ARG_ARRAY_INT64\
	|| arg==ARG_ARRAY_FLOAT\
	|| arg==ARG_ARRAY_DOUBLE\
	|| arg==ARG_ARRAY_STRING)

#define GET_ARRAY_TYPE(arg) (\
	arg==ARG_ARRAY_UINT8?ARG_UINT8:\
	arg==ARG_ARRAY_UINT16?ARG_UINT16:\
	arg==ARG_ARRAY_UINT32?ARG_UINT32:\
	arg==ARG_ARRAY_UINT64?ARG_UINT64:\
	arg==ARG_ARRAY_INT8?ARG_INT8:\
	arg==ARG_ARRAY_INT16?ARG_INT16:\
	arg==ARG_ARRAY_INT32?ARG_INT32:\
	arg==ARG_ARRAY_INT64?ARG_INT64:\
	arg==ARG_ARRAY_FLOAT?ARG_FLOAT:\
	arg==ARG_ARRAY_DOUBLE?ARG_DOUBLE:\
	arg==ARG_ARRAY_STRING?ARG_STRING:ARG_PROP)

#define ARG_IS_DICT(arg) (arg==ARG_DICT\
	|| arg==ARG_DICT_STRING_UINT8\
	|| arg==ARG_DICT_STRING_UINT16\
	|| arg==ARG_DICT_STRING_UINT32\
	|| arg==ARG_DICT_STRING_UINT64\
	|| arg==ARG_DICT_STRING_INT8\
	|| arg==ARG_DICT_STRING_INT16\
	|| arg==ARG_DICT_STRING_INT32\
	|| arg==ARG_DICT_STRING_INT64\
	|| arg==ARG_DICT_STRING_FLOAT\
	|| arg==ARG_DICT_STRING_DOUBLE\
	|| arg==ARG_DICT_STRING_STRING)

#define GET_DICT_KEY_TYPE(arg) (arg==ARG_DICT?ARG_PROP:ARG_STRING)

#define GET_DICT_VAL_TYPE(arg) (\
	arg==ARG_DICT_STRING_UINT8?ARG_UINT8:\
	arg==ARG_DICT_STRING_UINT16?ARG_UINT16:\
	arg==ARG_DICT_STRING_UINT32?ARG_UINT32:\
	arg==ARG_DICT_STRING_UINT64?ARG_UINT64:\
	arg==ARG_DICT_STRING_INT8?ARG_INT8:\
	arg==ARG_DICT_STRING_INT16?ARG_INT16:\
	arg==ARG_DICT_STRING_INT32?ARG_INT32:\
	arg==ARG_DICT_STRING_INT64?ARG_INT64:\
	arg==ARG_DICT_STRING_FLOAT?ARG_FLOAT:\
	arg==ARG_DICT_STRING_DOUBLE?ARG_DOUBLE:\
	arg==ARG_DICT_STRING_STRING?ARG_STRING:ARG_PROP)

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
	ARG_TYPE args_arr[] = args;\
	if (sizeof(args_arr) > 0)\
		map[id] = std::vector<ARG_TYPE>( args_arr, args_arr + sizeof(args_arr)/sizeof(args_arr[0]) );\
	else\
		map[id] = std::vector<ARG_TYPE>();\
}

#endif // FIXEDMESSAGES_H

