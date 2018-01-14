#ifndef LRUPOLICY_H
#define LRUPOLICY_H

#include "Policy.h"
#include <ctime>

class LRUPolicy: public Policy {
public:

	//necessary inputs: 
	//dataObject
	//minimum age of files to be migrated in seconds (i.e. move files older than minAge seconds)
    bool isFileKept(FileData dataObject, long int minAge) {

		long int currentTime = time(NULL);

		//if last access is before current time minus minimum age of files to be migrated
		if (dataObject.lastAccessed <= (currentTime - minAge))
		{
			//migrate the file
			return false;
		}
		else
		{
			//keep the file
			return true;
		}

    }
    bool isFileKept(std::list<FileData> dataList) {

    }
};
#endif
