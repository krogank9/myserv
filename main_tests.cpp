#include <iostream>
#include <vector>
#include <map>
#include "property.h"
#include "arg_stream.h"
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
		return false;\
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

	PRINT("\ntesting numbers:\n");

	DO(property num = 5);

	ASSERT(hi_str != num);
	ASSERT(num != 4);
	ASSERT(num == 5);
	ASSERT(num+1 == 5+1);

	DO(num += 5);
	ASSERT(num == 10);

	DO(property decimal = 1.5);
	ASSERT(decimal != num);
	DO(decimal += 8.5);
	ASSERT(decimal == num);

	DO(num += decimal);
	ASSERT(num.has_point());
	ASSERT(num == 20.0);

	PRINT("\ntesting array:\n");

	DO(std::vector<property> vec);
	DO(vec.push_back(property(1)));
	DO(vec.push_back(property(2)));
	DO(vec.push_back(property(3.0)));
	DO(vec.push_back(property('a')));

	DO(property arr = vec);
	ASSERT(arr[0] == 1.0);
	ASSERT(arr[1] == 2);
	ASSERT(arr[2] == 3);
	ASSERT(arr[3] == 'a');

	DO(property arr2 = vec);
	DO(property arr3 = arr2);
	ASSERT(arr == arr2);
	ASSERT(arr3 == arr2);

	PRINT("\ntesting dict:\n");

	DO(property dict;)
	DO(dict.set_dict());

	PRINT(dict.get_dict().size());

	DO(dict[0] = 0;)
	DO(dict["a"] = 3;)
	DO(dict["b"] = 3;)

	PRINT(dict.get_dict().size());

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

	DO(args.put_float(10.5));
	DO(args.put_double(11.5));
	ASSERT(args.get_float() == 10.5);
	ASSERT(args.get_double() == 11.5);

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
	DO(prop_vec.push_back(3.5));

	DO(prop_vec.push_back(3.5));

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
	DO(prop_dict[0.5] = 67);

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

int main()
{
	PRINT("doing property tests\n"HR);
	if (test_property())
	{
		PRINT(HR"\nall property tests passed.");
	}
	else
	{
		PRINT(HR"\nproperty tests failed!");
		return 1;
	}

	PRINT("\ndoing arg_stream tests\n"HR);
	if (test_args_stream())
	{
		PRINT(HR"\nall arg_stream tests passed.");
	}
	else
	{
		PRINT(HR"\narg_stream tests failed!");
		return 1;
	}

	PRINT("\n"HR"\nall tests passed.");
}
