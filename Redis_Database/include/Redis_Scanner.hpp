#ifndef Redis_Scanner_H
#define Redis_Scanner_H


#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "stdlib.h"

//#include "redox.hpp"
#include "Redis_Client.hpp"
#include "../../lib/FileData.hpp"
#include "../../lib/SizePolicy.hpp"
#include "../../lib/TimePolicy.hpp"
//#include "../../lib/HitPolicy.hpp"


using namespace std;
//using redox::Redox;
//using redox::Command;
//using redox::Subscriber;


class Redis_Scanner
{

public:
	void addToFileSizeSet(FileData&);
	vector<FileData> getFilesInSizeRange(SizePolicy);
	
	void addToLastModifiedSet(FileData& file);
	vector<FileData> getFilesWithinLastModifiedTime(TimePolicy);

	//void addToHitList(FileData&);
};

#endif