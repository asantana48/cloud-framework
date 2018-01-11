#ifndef POLICYMANAGER_H
#define POLICYMANAGER_H

#include "Policy.h"
#include "SizePolicy.h"
#include "LRUPolicy.h"
#include "FileData.h"

#include <list>
#include <string>
#include <algorithm>

class PolicyManager {
private:
    std::list<Policy*> policyList;
public:
    PolicyManager() {}
    ~PolicyManager();

    void addPolicy(Policy* p);

    std::string getListString(); // Temp

};
#endif
