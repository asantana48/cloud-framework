#ifndef LRUPOLICY_H
#define LRUPOLICY_H

#include "Policy.h"
class LRUPolicy: public Policy {
public:
    // TODO add logic
    bool isFileKept(FileData dataObject) {
        return false;
    }
    bool isFileKept(std::list<FileData> dataList) {

    }
};
#endif
