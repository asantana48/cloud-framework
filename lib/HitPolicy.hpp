#ifndef HITSPOLICY_H
#define HITSPOLICY_H

#include "FileData.hpp"
#include "Policy.hpp"

class HitPolicy : public Policy {
private:
    int hits;

public:
    HitPolicy::HitPolicy(int numHits)
    {
        hits = numHits;
    }

    bool isFileKept(FileData dataObject)
    {
        if (hits > dataObject.timesAccessed)
            return true;
        else
            return false;
    }
};
#endif