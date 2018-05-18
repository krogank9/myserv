#include <iostream>
#include <vector>
#include <map>
#include "property.h"

using namespace std;

#define HR "--------------------------"

#define STRING(s) #s
#define ASSERT(cond)\
{\
	if((cond))\
	{\
		cout << "ASSERT("STRING(cond)") passed" <<  endl;\
	}\
	else if(!(cond))\
	{\
		cout << "ASSERT("STRING(cond)") failed" << endl;\
		return false;\
	}\
}
#define PRINT(str) cout << str << endl
#define DO(statement)\
	statement; PRINT(STRING(statement));

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

	PRINT("\n"HR"\nall tests passed.");
}
