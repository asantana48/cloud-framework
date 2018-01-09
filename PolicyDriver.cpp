#include "lib/PolicyManager.h"
#include "lib/SizePolicy.h"
#include "lib/LRUPolicy.h"

#include <iostream>

using namespace std;

int main() {
  PolicyManager pm;
  SizePolicy sp;
  LRUPolicy lp1, lp2;

  pm.addPolicy(new SizePolicy());
  pm.addPolicy(new SizePolicy());
  pm.addPolicy(new LRUPolicy());

  std::cout << pm.getListString() << std::endl;

  return 0;
}