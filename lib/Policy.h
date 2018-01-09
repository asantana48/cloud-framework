#ifndef POLICY_H
#define POLICY_H

#include <string>
class Policy {
public:
    // TODO - add file type paramater
    virtual bool isFileKept() = 0;
};
#endif
