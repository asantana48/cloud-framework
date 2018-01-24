#ifndef SIZEPOLICY_H
#define SIZEPOLICY_H

#include "FileData.h"
#include "Policy.h"

class SizePolicy : public Policy {
private:
	int threshold;
    bool greaterThan;

public:
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
