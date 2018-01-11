#ifndef SIZEPOLICY_H
#define SIZEPOLICY_H

#include "FileData.h"
#include "Policy.h"

class SizePolicy : public Policy {
private:
	int threshold;

public:
	SizePolicy::SizePolicy(int newThreshold)
	{
		threshold = newThreshold;
	}


	bool isFileKept(FileData dataObject) 
	{
		if (dataObject.fileSize < threshold)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};
#endif
