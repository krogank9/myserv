#include <iostream>
#include "property.h"

using namespace std;

#define STRING(s) #s
#define ASSERT(cond)\
{\
	if((cond))\
	{\
		cout << "("STRING(cond)") passed" <<  endl;\
	}\
	else if(!(cond))\
	{\
		cout << "("STRING(cond)") failed" << endl;\
		return false;\
	}\
}

bool test_property()
{
	property str1 = "hi";
	property str2 = "bye";
	property str3 = "hi";
	ASSERT(str1 != str2);
	ASSERT(str1 == str3);
	return true;
}

int main()
{
	if (test_property())
	{
		cout << "all property tests passed." << endl;
	}
	else
	{
		cout << "property tests failed!" << endl;
		return 1;
	}
}
