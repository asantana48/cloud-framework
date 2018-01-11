#ifndef POLICY_H
#define POLICY_H

#include "FileData.h"

class Policy {
public:
    // TODO - add file type paramater
    virtual bool isFileKept(FileData fd) = 0;
};
#endif
