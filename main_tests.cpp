#include <iostream>
#include "property.h"

using namespace std;

int main()
{
	property p = 5;
	p+=6;

	cout << p.get_string() << endl;
}
