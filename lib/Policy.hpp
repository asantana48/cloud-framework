#ifndef POLICY_H
#define POLICY_H

#include <list>
#include "FileData.hpp"

class Policy {
public:
    virtual ~Policy() {}
    
    // Make a policy decision based on some absolute value
    virtual bool isFileKept(FileData fd) { return true; }

    // Make a policy decision relative to other files
    virtual bool isFileKept(std::list<FileData> fl) { return true; }
};
#endif