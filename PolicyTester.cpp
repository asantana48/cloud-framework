#include "lib/PolicyManager.h"
#include "lib/SizePolicy.h"
#include "lib/LRUPolicy.h"

#include <iostream>

using namespace std;

int main() {
  PolicyManager pm;
  SizePolicy* sp = new SizePolicy(5);
  LRUPolicy* lp1 = new LRUPolicy();
  LRUPolicy* lp2 = new LRUPolicy();

  pm.addPolicy(sp);
  pm.addPolicy(lp1);
  pm.addPolicy(lp2);

  delete sp, lp1, lp2;

  std::cout << pm.getListString() << std::endl;

  return 0;
}