#ifndef LRUPOLICY_H
#define LRUPOLICY_H

#include "Policy.h"
class LRUPolicy: public Policy {
public:
    // TODO add logic
    bool isFileKept() {
        return false;
    }
};
#endif
