#ifndef POLICYMANAGER_H
#define POLICYMANAGER_H

#include "Policy.h"
#include "SizePolicy.h"
#include "TimePolicy.h"
#include "FileData.h"

#include <list>
#include <string>
#include <algorithm>

class PolicyManager {
private:
    std::list<Policy*> policyList;
    
    // Out of 100
    int saturation;
public:
    PolicyManager() {}
    ~PolicyManager();

    void addPolicy(Policy* p);
    void setMaxSaturation(int saturation);
	bool isFilePromoted(FileData fd);
	std::list<FileData> getFileDemotionList(std::list<FileData> fileList); 

};
#endif
