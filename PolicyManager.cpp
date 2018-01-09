#include "lib/PolicyManager.h"

PolicyManager::~PolicyManager()
{
    std::list<Policy*>::iterator it = policyList.begin();
    while (it != policyList.end()) {
        delete *it;
        it = policyList.erase(it);
    }
}

void PolicyManager::addPolicy(Policy* p)
{
    policyList.push_back(p);
}



std::string PolicyManager::getListString()
{
    std::string reply = "";
    std::list<Policy*>::iterator it;
    for (it = policyList.begin(); it != policyList.end(); ++it) {
        bool contents = (*it)->isFileKept();
        reply += (contents ? "Size " : "LRU ");
    }
    return reply;
}
