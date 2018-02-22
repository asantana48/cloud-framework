#ifndef POLICY_H
#define POLICY_H

#include <string>
#include <list>
#include "FileData.hpp"

class Policy {
public:
    virtual ~Policy() {}
    
    // Name of the policy in question
    std::string name;

    // Type of policy
    std::string type;

    // Make a policy decision based on some absolute value
    virtual bool isFileKept(FileData fd) { return true; }
};
#endif