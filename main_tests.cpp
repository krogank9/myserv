#include <iostream>
#include <vector>
#include <map>
#include "property.h"
#include "arg_stream.h"
#include "fixed_messages.h"
#include "message_reader.h"
#include "tcp_connection.h"
#include "stdint.h"

using namespace std;

#define HR "--------------------------"

#define SINGLE_ARG(...) __VA_ARGS__
#define STRING(...) #__VA_ARGS__
#define ASSERT(cond)\
{\
	if(cond)\
	{\
		cout << "ASSERT("STRING(cond)") passed" <<  endl;\
	}\
	else\
	{\
		cout << "ASSERT("STRING(cond)") failed" << endl;\
		exit(1);\
	}\
}
#define PRINT(str) cout << str << endl
// needed varargs magic to handle printing statements with commas
#define _DO(statement)\
	statement; PRINT(STRING(statement)";");
#define DO(...) _DO(SINGLE_ARG(__VA_ARGS__))

bool test_property()
{
	PRINT("testing strings:\n");

	DO(property hi_str = "hi");
	DO(property bye_str = "bye");
	DO(property hi_str_2 = "hi");

	ASSERT(hi_str != bye_str);
	ASSERT(hi_str == hi_str_2);

	DO(hi_str += " world");
	ASSERT(hi_str == "hi world");

	PRINT("\ntesting numbers:\n");

	DO(property num = 5);

	ASSERT(hi_str != num);
	ASSERT(num != 4);
	ASSERT(num == 5);
	ASSERT(num+1 == 5+1);

	DO(num += 5);
	ASSERT(num == 10);

	DO(property decimal = 1.5f);
	ASSERT(decimal != num);
	DO(decimal += 8.5f);
	ASSERT(decimal == num);

	DO(num += decimal);
	ASSERT(num.has_point());
	ASSERT(num == 20.0f);

	PRINT("\ntesting array:\n");

	DO(std::vector<property> vec);
	DO(vec.push_back(property(1)));
	DO(vec.push_back(property(2)));
	DO(vec.push_back(property(3.0f)));
	DO(vec.push_back(property('a')));

	DO(property arr = vec);
	ASSERT(arr[0] == 1.0f);
	ASSERT(arr[1] == 2);
	ASSERT(arr[2] == 3);
	ASSERT(arr[3] == 'a');

	DO(property arr2 = vec);
	DO(property arr3 = arr2);
	ASSERT(arr == arr2);
	ASSERT(arr3 == arr2);

	PRINT("\ntesting dict:\n");

	DO(property dict);
	DO(dict.set_dict());

	DO(dict[0] = 0);
	DO(dict["a"] = 3);
	DO(dict["b"] = 3);

	ASSERT(dict.get_dict().size() == 3);
	ASSERT(dict[0] == 0);
	ASSERT(dict["a"] == 3);

	DO(dict["b"] = 5);
	ASSERT(dict["b"] == 5);

	PRINT("\ntesting blob\n");

	DO(property blob);
	DO(blob.set_blob());

	DO(blob.get_blob().push_back(0));
	DO(blob.get_blob().push_back(0));
	DO(blob.get_blob().push_back(0));

	ASSERT(blob.get_blob().size() == 3);

	return true;
}

bool test_args_stream()
{
	DO(arg_stream args);

	PRINT("\ntesting primitive types:\n");

	DO(args.put_8(10));
	DO(args.put_16(1000));
	ASSERT(args.get_8() == 10);
	ASSERT(args.get_16() == 1000);

	DO(args.put_32(10000));
	DO(args.put_64(100000));
	ASSERT(args.get_32() == 10000);
	ASSERT(args.get_64() == 100000);

	DO(args.put_u8(10));
	DO(args.put_u16(10));
	DO(args.put_u32(999));
	DO(args.put_u64(5));
	ASSERT(args.get_u8() == 10);
	ASSERT(args.get_u16() == 10);
	ASSERT(args.get_u32() == 999);
	ASSERT(args.get_u64() == 5);

	DO(args.put_float(10.5f));
	DO(args.put_double(11.5f));
	ASSERT(args.get_float() == 10.5f);
	ASSERT(args.get_double() == 11.5f);

	PRINT("\ntesting blob:\n");

	char b[5] = {1,2,3,4,5};
	char b2[5] = {5,4,3,2,1};
	std::vector<char> b_vec(b, b+5);
	std::vector<char> b2_vec(b2, b2+5);
	DO(args.put_blob(b, 5));
	DO(args.put_8(5));
	DO(args.put_blob(b2_vec));
	DO(std::vector<char> blob_read = args.get_blob());
	ASSERT(blob_read == b_vec);
	ASSERT(args.get_8() == 5);
	DO(blob_read = args.get_blob());
	ASSERT(blob_read != b_vec);
	ASSERT(blob_read == b2_vec);

	PRINT("\ntesting string:\n");

	DO(args.clear());

	DO(args.put_string("abc"));
	DO(args.put_string("test"));
	DO(args.put_string("p"));

	ASSERT(args.get_string() == "abc");
	ASSERT(args.get_string() == "test");
	ASSERT(args.get_string() == "p");

	PRINT("\ntesting property:\n");

	DO(args.clear());

	DO(property p_num = 5);
	DO(property p_str = "test");

	DO(args.put_property(p_num));
	DO(args.put_property(p_str));

	ASSERT(args.get_property() == p_num);
	ASSERT(args.get_property() == p_str);

	PRINT("\ntesting array:\n");

	DO(std::vector<property> prop_vec);
	DO(prop_vec.push_back(0));
	DO(prop_vec.push_back("a"));
	DO(prop_vec.push_back(3.5f));

	DO(prop_vec.push_back(3.5f));

	DO(args.put_array(prop_vec));
	DO(std::vector<property> read_prop_vec = args.get_array());
	ASSERT(read_prop_vec == prop_vec);
	ASSERT(read_prop_vec[1] == "a");

	DO(property prop_prop_vec = prop_vec);
	DO(args.put_property(prop_prop_vec));
	ASSERT(args.get_property() == prop_prop_vec);

	PRINT("\ntesting dict:\n");

	DO(std::map<property, property> prop_dict);
	DO(std::map<property, property> dummy_dict);
	DO(prop_dict["hi"] = 5);
	DO(prop_dict[0] = 5);
	DO(prop_dict[0.5f] = 67);

	DO(args.put_dict(prop_dict));
	DO(std::map<property, property> read_prop_dict = args.get_dict());
	ASSERT(read_prop_dict == prop_dict);
	ASSERT(read_prop_dict != dummy_dict);

	DO(property prop_prop_dict = prop_dict);
	DO(args.put_property(prop_prop_dict));
	ASSERT(args.get_property() == prop_prop_dict);

	PRINT("\ntesting typed dict+array:\n");

	DO(std::vector<property> typed_prop_arr);
	DO(std::map<property, property> typed_prop_dict);
	DO(typed_prop_arr.push_back((int8_t)0));
	DO(typed_prop_arr.push_back((int8_t)1));
	DO(typed_prop_arr.push_back((int8_t)2));
	DO(typed_prop_arr.push_back((int8_t)3));

	DO(typed_prop_dict["a"] = (int8_t)1);
	DO(typed_prop_dict["b"] = (int8_t)2);
	DO(typed_prop_dict["c"] = (int8_t)3);
	DO(typed_prop_dict["d"] = (int8_t)4);

	DO(args.put_array(typed_prop_arr, ARG_INT8));
	DO(args.put_dict(typed_prop_dict, ARG_STRING, ARG_INT8));

	ASSERT(args.get_array(ARG_INT8) == typed_prop_arr);
	ASSERT(args.get_dict(ARG_STRING, ARG_INT8) == typed_prop_dict);

	return true;
}

#define EMPTY_MSG_ID 0
#define STRING_MSG_ID 1
#define NUM_MSG_ID 2
#define PROP_MSG_ID 3
#define CONTAINER_MSG_ID 4
property mr_prop1 = 123;
property mr_prop2 = "abc";
std::vector<property> mr_p_vec;
property prop_mr_p_vec;
std::map<property, property> mr_p_map;
std::vector<char> mr_blob_vec;
bool test_message_reader()
{
	mr_p_vec.push_back(123);
	mr_p_vec.push_back("abc");
	mr_p_vec.push_back(10.5f);
	mr_p_vec.push_back(-1000);
	mr_p_vec.push_back(mr_p_vec);
	prop_mr_p_vec = mr_p_vec;

	mr_p_map["abc"] = 5;
	mr_p_map["asassd"] = 5.555f;
	mr_p_map[5] = 3;
	mr_p_map["nested"] = mr_p_map;

	mr_blob_vec.push_back(0);
	mr_blob_vec.push_back(1);
	mr_blob_vec.push_back(3);
	class dummy_handler : public message_handler
	{
	public:
		dummy_handler()
		{
			string_msg.push_back(ARG_STRING);
			string_msg.push_back(ARG_STRING);

			num_msg.push_back(ARG_FLOAT);
			num_msg.push_back(ARG_DOUBLE);
			num_msg.push_back(ARG_UINT8);
			num_msg.push_back(ARG_INT64);

			prop_msg.push_back(ARG_PROP);
			prop_msg.push_back(ARG_PROP);

			container_msg.push_back(ARG_ARRAY);
			container_msg.push_back(ARG_DICT);
			container_msg.push_back(ARG_BLOB);
			container_msg.push_back(ARG_PROP);
		}

		bool call_network_interface(tcp_connection* originator, int msgID, arg_stream& args)
		{
			if (msgID == EMPTY_MSG_ID)
			{
				PRINT("received empty msg");
				ASSERT(args.length() == 0);
			}
			else if (msgID == STRING_MSG_ID)
			{
				PRINT("received string msg");
				ASSERT(args.get_string() == "str1");
				ASSERT(args.get_string() == "test");
				ASSERT(args.length() == 0);
			}
			else if (msgID == NUM_MSG_ID)
			{
				PRINT("received num msg");
				ASSERT(args.get_float() == 0.1f);
				ASSERT(args.get_double() == 0.01f);
				ASSERT(args.get_u8() == 10);
				ASSERT(args.get_64() == 1000);
				ASSERT(args.length() == 0);
			}
			else if (msgID == PROP_MSG_ID)
			{
				PRINT("received prop msg");
				ASSERT(args.get_property() == mr_prop1);
				ASSERT(args.get_property() == mr_prop2);
				ASSERT(args.length() == 0);
			}
			else if (msgID == CONTAINER_MSG_ID)
			{
				PRINT("received container msg");
				ASSERT(args.get_array() == mr_p_vec);
				ASSERT(args.get_dict() == mr_p_map);
				ASSERT(args.get_blob() == mr_blob_vec);
				ASSERT(args.get_property() == prop_mr_p_vec);
				ASSERT(args.length() == 0);
			}
			return true;
		}

		std::vector<ARG_TYPE>* get_msg_args_by_id(MSG_ID msgID)
		{
			if (msgID == EMPTY_MSG_ID)
				return &empty_msg;
			if (msgID == STRING_MSG_ID)
				return &string_msg;
			if (msgID == NUM_MSG_ID)
				return &num_msg;
			if (msgID == PROP_MSG_ID)
				return &prop_msg;
			if (msgID == CONTAINER_MSG_ID)
				return &container_msg;
			return NULL;
		}

		vector<ARG_TYPE> empty_msg;
		vector<ARG_TYPE> string_msg;
		vector<ARG_TYPE> num_msg;
		vector<ARG_TYPE> prop_msg;
		vector<ARG_TYPE> container_msg;
	} dummy;
	message_reader reader(&dummy, NULL);

	arg_stream args;

	// empty msg
	args.put_msg_id(EMPTY_MSG_ID);
	// string msg
	args.put_msg_id(STRING_MSG_ID);
	args.put_string("str1");
	args.put_string("test");
	// num msg
	args.put_msg_id(NUM_MSG_ID);
	args.put_float(0.1f);
	args.put_double(0.01f);
	args.put_u8(10);
	args.put_64(1000);
	// prop msg
	args.put_msg_id(PROP_MSG_ID);
	args.put_property(mr_prop1);
	args.put_property(mr_prop2);
	// container msg
	args.put_msg_id(CONTAINER_MSG_ID);
	args.put_array(mr_p_vec);
	args.put_dict(mr_p_map);
	args.put_blob(mr_blob_vec);
	args.put_property(prop_mr_p_vec);

	ASSERT( reader.process(args.get_buffer(), args.length()) == true );

	return true;
}

int main()
{
	PRINT("doing property tests\n"HR);
	if (test_property())
	{
		PRINT(HR"\nall property tests passed.");
	}


	PRINT("\ndoing arg_stream tests\n"HR);
	if (test_args_stream())
	{
		PRINT(HR"\nall arg_stream tests passed.");
	}

	PRINT("\ndoing message_reader tests\n"HR);
	if (test_message_reader())
	{
		PRINT(HR"\nall message_reader tests passed.");
	}

	PRINT("\n"HR"\nall tests passed.");
}
