#ifndef SIZEPOLICY_H
#define SIZEPOLICY_H

#include "FileData.hpp"
#include "Policy.hpp"

class SizePolicy : public Policy {
private:
	int lowThreshold;
	int highThreshold;

public:
	//If migrateGreater == true, file will be migrated if greater than threshold. Otherwise, if smaller than threshold.
	SizePolicy(int lowThreshold, int highThreshold) :
		lowThreshold(lowThreshold), highThreshold(highThreshold) {}

	int getHighThreshold(){ return highThreshold;}

	int getLowThreshold(){ return lowThreshold;}

	/*
	bool isFileKept(FileData dataObject) 
	{
		if (greaterThan)
			return dataObject.fileSize <= threshold;
		else
			return dataObject.fileSize >= threshold;
	}
	*/
};
#endif
