#ifndef HITSPOLICY_H
#define HITSPOLICY_H

#include "FileData.h"
#include "Policy.h"

class HitsPolicy : public Policy {
private:
    int hits;

public:
    HitsPolicy::HitsPolicy(int numHits)
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