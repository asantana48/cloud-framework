#include "PolicyManager.h"
#include "SizePolicy.h"
#include "TimePolicy.h"

#include <iostream>

using namespace std;

int main()
{
	PolicyManager pm;
	SizePolicy* sp = new SizePolicy(5, true);
	TimePolicy* lp1 = new TimePolicy(500);
	TimePolicy* lp2 = new TimePolicy(600);

	pm.addPolicy(sp);
	pm.addPolicy(lp1);
	pm.addPolicy(lp2);

	//delete sp, lp1, lp2;

	std::cout << pm.getListString() << std::endl;

	return 0;
}
