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
#include "../../lib/HitPolicy.hpp"


using namespace std;
//using redox::Redox;
//using redox::Command;
//using redox::Subscriber;


class Redis_Scanner
{

public:
	static bool orderFiles(FileData&, FileData&);
	void sortVector(vector<FileData>& files);

	void addToFileSizeSet(FileData&);
	vector<FileData> getFilesInSizeRange(SizePolicy);
	vector<FileData> getFilesOutOfSizeRange(SizePolicy);
	
	void addToLastModifiedSet(FileData&);
	vector<FileData> getFilesInLastModifiedTime(TimePolicy);
	vector<FileData> getFilesOutOfLastModifiedTime(TimePolicy);


	void addToHitList(FileData&);
	vector<FileData> getFilesInTimesAccessedRange(HitPolicy);
	vector<FileData> getFilesOutOfTimesAccessedRange(HitPolicy);


	void addToIsLocalList(FileData&);
	void changeLocalToNonLocal(FileData&);
	void changeNonLocalToLocal(FileData&);
	vector<FileData> getLocalFiles();
	vector<FileData> getNonLocalFiles();


};

#endif