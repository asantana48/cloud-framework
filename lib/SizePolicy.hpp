#ifndef SIZEPOLICY_H
#define SIZEPOLICY_H

#include "FileData.hpp"
#include "Policy.hpp"

class SizePolicy : public Policy {
private:
	int threshold;
    	bool greaterThan;

public:
	//If migrateGreater == true, file will be migrated if greater than threshold. Otherwise, if smaller than threshold.
	SizePolicy::SizePolicy(int newThreshold, bool migrateGreater)
	{
		threshold = newThreshold;
        	greaterThan = migrateGreater;
	}


	bool isFileKept(FileData dataObject) 
	{
		if (greaterThan)
			return dataObject.fileSize <= threshold;
		else
			return dataObject.fileSize >= threshold;
	}
};
#endif
