#ifndef FILEDATA_H
#define FILEDATA_H
#include <string>
#include <sstream>
#include <ctime>


struct FileData
{
	int fileSize;
	time_t lastModified;


	std::string getFileSizeAsString()
	{
        std::stringstream s;
		s << fileSize;
		return s.str();
	}

    std::string getLastModifiedAsString()
	{
        std::stringstream s;
		s << lastModified;
		return s.str();
	}
};
#endif
