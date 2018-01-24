#ifndef FILEDATA_H
#define FILEDATA_H
#include <string>
#include <sstream>
#include <ctime>


struct FileData
{
    std::string location;
	int fileSize;
    int timesAccessed;
	time_t lastModified;

    bool isLocal;

    void setFileSize(std::string s) {
        fileSize = stoi(s);
    }

    void setTimesAccessed(std::string s) {
        timesAccessed = stoi(s);
        strptime(s.c_str(), %H &tmm)
    }

    void setLastModified(std::string s) {
        struct time_tm;
    }
    void setIsLocal(std::string s) {
        if (s.compare("true") == 0)
            isLocal = true;
        else if (s.compare("false") == 0)
            isLocal = false;
    }
	std::string getFileSize()
	{
        std::stringstream s;
		s << fileSize;
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
    }

};
#endif
