#ifndef FILEDATA_H
#define FILEDATA_H
#include <string>
#include <sstream>
#include <ctime>


struct FileData
{
    std::string localURI;
    std::string fileName;
    std::string remoteURI; // TODO make it more relevant - i.e. contain cloud, bucket info

    int fileSize;
    int timesAccessed; 
    time_t lastModified;
    bool isLocal;
    bool isMetadata;
    bool isOpen;

	std::string getFileSize()
	{
        std::stringstream s;
		s << fileSize;
		return s.str();
	}

    std::string getTimesAccessed()
	{
        std::stringstream s;
		s << timesAccessed;
		return s.str();
	} 
    std::string getLastModified()
	{
        std::stringstream s;
		s << lastModified;
		return s.str();
	}

    std::string getIsLocal() {
        std::stringstream s;
        s << isLocal;
        return s.str();
    }

    std::string getIsMetadata() {
        std::stringstream s;
        s << isMetadata;
        return s.str();
    }

    std::string getIsOpen() {
        std::stringstream s;
        s << isOpen;
        return s.str();
    }
};
#endif
