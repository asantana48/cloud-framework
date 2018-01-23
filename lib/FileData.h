#ifndef FILEDATA_H
#define FILEDATA_H
#include <string>
#include <sstream>
#include <ctime>

using namespace std;

struct FileData
{
	int fileSize;
	time_t lastModified;


	string getFileSizeAsString()
	{
		stringstream s;
		s << fileSize;
		return s.str();
	}

	string getLastModifiedAsString()
	{
		stringstream s;
		s << lastModified;
		return s.str();
	}



};
#endif
