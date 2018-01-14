#ifndef POLICY_H
#define POLICY_H

#include "FileData.h"

class Policy {
public:
    ~Policy() {}
    // TODO - add file type paramater
    virtual bool isFileKept(FileData fd) = 0;
};
#endif
